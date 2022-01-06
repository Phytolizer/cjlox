#ifndef PHYTO_HASH_HASH_H_
#define PHYTO_HASH_HASH_H_

#include <phyto/string/string.h>
#include <phyto/string_view/string_view.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PHYTO_HASH_FLAGS_X                        \
    X(ok, "ok")                                   \
    X(error, "error")                             \
    X(duplicate, "duplicate key")                 \
    X(empty, "empty hash")                        \
    X(not_found, "key not found")                 \
    X(overflow, "overflow calculating next size") \
    X(alloc, "allocation failure")

typedef enum {
#define X(x, y) phyto_hash_flag_##x,
    PHYTO_HASH_FLAGS_X
#undef X
} phyto_hash_flag_t;

phyto_string_view_t phyto_hash_flag_explain(phyto_hash_flag_t flag);

typedef PHYTO_SPAN_TYPE(uint64_t) phyto_hash_prime_span_t;

phyto_hash_prime_span_t phyto_hash_prime_span(void);

extern const double phyto_hash_default_load;

#define PHYTO_HASH_DECL(Name, V)                                                               \
    typedef enum {                                                                             \
        Name##_entry_state_empty,                                                              \
        Name##_entry_state_filled,                                                             \
        Name##_entry_state_deleted,                                                            \
    } Name##_entry_state_t;                                                                    \
    typedef struct {                                                                           \
        phyto_string_t key;                                                                    \
        V value;                                                                               \
        Name##_entry_state_t state;                                                            \
    } Name##_entry_t;                                                                          \
    typedef struct {                                                                           \
        uint64_t (*hash)(phyto_string_view_t);                                                 \
    } Name##_key_ops_t;                                                                        \
    typedef struct {                                                                           \
        int32_t (*compare)(V, V);                                                              \
        V (*copy)(V);                                                                          \
        bool (*print)(FILE*, V);                                                               \
        void (*free)(V);                                                                       \
        uint64_t (*hash)(V);                                                                   \
    } Name##_value_ops_t;                                                                      \
    typedef struct {                                                                           \
        Name##_entry_t* buffer;                                                                \
        size_t capacity;                                                                       \
        size_t count;                                                                          \
        double load;                                                                           \
        phyto_hash_flag_t flag;                                                                \
        const Name##_key_ops_t* key_ops;                                                             \
        const Name##_value_ops_t* value_ops;                                                         \
    } Name##_t;                                                                                \
    Name##_t* Name##_new(size_t capacity, double load, const Name##_key_ops_t* key_ops,        \
                         const Name##_value_ops_t* value_ops);                                 \
    void Name##_clear(Name##_t* map);                                                          \
    void Name##_free(Name##_t* map);                                                           \
    bool Name##_insert(Name##_t* map, phyto_string_view_t key, V value);                       \
    bool Name##_update(Name##_t* map, phyto_string_view_t key, V new_value, V* out_old_value); \
    bool Name##_remove(Name##_t* map, phyto_string_view_t key, V* out_value);                  \
    bool Name##_max(Name##_t* map, phyto_string_t* out_key, V* out_value);                     \
    bool Name##_min(Name##_t* map, phyto_string_t* out_key, V* out_value);                     \
    V Name##_get(Name##_t* map, phyto_string_view_t key);                                      \
    V* Name##_get_ref(Name##_t* map, phyto_string_view_t key);                                 \
    bool Name##_contains(Name##_t* map, phyto_string_view_t key);                              \
    bool Name##_empty(Name##_t* map);                                                          \
    bool Name##_full(Name##_t* map);                                                           \
    size_t Name##_count(Name##_t* map);                                                        \
    size_t Name##_capacity(Name##_t* map);                                                     \
    double Name##_load(Name##_t* map);                                                         \
    phyto_hash_flag_t Name##_flag(Name##_t* map);                                              \
    bool Name##_resize(Name##_t* map, size_t new_capacity);                                    \
    Name##_t* Name##_copy_of(Name##_t* map);                                                   \
    bool Name##_equals(Name##_t* map, Name##_t* other)

#define PHYTO_HASH_IMPL(Name, V)                                                                \
    static Name##_entry_t* Name##_impl_get_entry(Name##_t* map, phyto_string_view_t key);       \
    static size_t Name##_impl_calculate_size(size_t required);                                  \
    Name##_t* Name##_new(size_t capacity, double load, const Name##_key_ops_t* key_ops,         \
                         const Name##_value_ops_t* value_ops) {                                 \
        if (capacity == 0 || load <= 0 || load >= 1) {                                          \
            return NULL;                                                                        \
        }                                                                                       \
        if ((double)capacity >= (double)UINTMAX_MAX * load) {                                   \
            return NULL;                                                                        \
        }                                                                                       \
        if (!value_ops) {                                                                       \
            return NULL;                                                                        \
        }                                                                                       \
        size_t real_capacity = Name##_impl_calculate_size((size_t)((double)capacity / load));   \
        Name##_t* map = malloc(sizeof(Name##_t));                                               \
        if (!map) {                                                                             \
            return NULL;                                                                        \
        }                                                                                       \
        map->buffer = calloc(real_capacity, sizeof(Name##_entry_t));                            \
        if (!map->buffer) {                                                                     \
            free(map);                                                                          \
            return NULL;                                                                        \
        }                                                                                       \
        map->count = 0;                                                                         \
        map->capacity = real_capacity;                                                          \
        map->load = load;                                                                       \
        map->flag = phyto_hash_flag_ok;                                                         \
        map->key_ops = key_ops;                                                                 \
        map->value_ops = value_ops;                                                             \
        return map;                                                                             \
    }                                                                                           \
    void Name##_clear(Name##_t* map) {                                                          \
        for (size_t i = 0; i < map->capacity; ++i) {                                            \
            Name##_entry_t* entry = &map->buffer[i];                                            \
            if (entry->state == Name##_entry_state_filled) {                                    \
                phyto_string_free(&entry->key);                                                 \
                if (map->value_ops->free) {                                                     \
                    map->value_ops->free(entry->value);                                         \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
        memset(map->buffer, 0, sizeof(Name##_entry_t) * map->capacity);                         \
        map->count = 0;                                                                         \
        map->flag = phyto_hash_flag_ok;                                                         \
    }                                                                                           \
    void Name##_free(Name##_t* map) {                                                           \
        for (size_t i = 0; i < map->capacity; ++i) {                                            \
            Name##_entry_t* entry = &map->buffer[i];                                            \
            if (entry->state == Name##_entry_state_filled) {                                    \
                phyto_string_free(&entry->key);                                                 \
                if (map->value_ops->free) {                                                     \
                    map->value_ops->free(entry->value);                                         \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
        free(map->buffer);                                                                      \
        free(map);                                                                              \
    }                                                                                           \
    bool Name##_insert(Name##_t* map, phyto_string_view_t key, V value) {                       \
        if (Name##_full(map)) {                                                                 \
            if (!Name##_resize(map, map->capacity + 1)) {                                       \
                return false;                                                                   \
            }                                                                                   \
        }                                                                                       \
        if (Name##_impl_get_entry(map, key) != NULL) {                                          \
            map->flag = phyto_hash_flag_duplicate;                                              \
            return false;                                                                       \
        }                                                                                       \
        size_t hash = map->key_ops->hash(key);                                                  \
        size_t original_pos = hash % map->capacity;                                             \
        size_t pos = original_pos;                                                              \
                                                                                                \
        Name##_entry_t* target = &map->buffer[pos];                                             \
        if (target->state == Name##_entry_state_empty ||                                        \
            target->state == Name##_entry_state_deleted) {                                      \
            target->key = phyto_string_own(key);                                                \
            target->value = value;                                                              \
            target->state = Name##_entry_state_filled;                                          \
        } else {                                                                                \
            while (true) {                                                                      \
                pos++;                                                                          \
                target = &map->buffer[pos % map->capacity];                                     \
                if (target->state == Name##_entry_state_empty ||                                \
                    target->state == Name##_entry_state_deleted) {                              \
                    target->key = phyto_string_own(key);                                        \
                    target->value = value;                                                      \
                    target->state = Name##_entry_state_filled;                                  \
                    break;                                                                      \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
        map->count++;                                                                           \
        map->flag = phyto_hash_flag_ok;                                                         \
        return true;                                                                            \
    }                                                                                           \
    bool Name##_update(Name##_t* map, phyto_string_view_t key, V new_value, V* out_old_value) { \
        if (Name##_empty(map)) {                                                                \
            map->flag = phyto_hash_flag_empty;                                                  \
            return false;                                                                       \
        }                                                                                       \
        Name##_entry_t* entry = Name##_impl_get_entry(map, key);                                \
        if (!entry) {                                                                           \
            map->flag = phyto_hash_flag_not_found;                                              \
            return false;                                                                       \
        }                                                                                       \
        if (out_old_value) {                                                                    \
            *out_old_value = entry->value;                                                      \
        }                                                                                       \
        entry->value = new_value;                                                               \
        map->flag = phyto_hash_flag_ok;                                                         \
        return true;                                                                            \
    }                                                                                           \
    bool Name##_remove(Name##_t* map, phyto_string_view_t key, V* out_value) {                  \
        if (Name##_empty(map)) {                                                                \
            map->flag = phyto_hash_flag_empty;                                                  \
            return false;                                                                       \
        }                                                                                       \
        Name##_entry_t* entry = Name##_impl_get_entry(map, key);                                \
        if (!entry) {                                                                           \
            map->flag = phyto_hash_flag_not_found;                                              \
            return false;                                                                       \
        }                                                                                       \
        if (out_value) {                                                                        \
            *out_value = entry->value;                                                          \
        }                                                                                       \
        phyto_string_free(&entry->key);                                                         \
        entry->value = (V){0};                                                                  \
        entry->state = Name##_entry_state_deleted;                                              \
        map->count--;                                                                           \
        map->flag = phyto_hash_flag_ok;                                                         \
        return true;                                                                            \
    }                                                                                           \
    bool Name##_max(Name##_t* map, phyto_string_t* out_key, V* out_value) {                     \
        if (Name##_empty(map)) {                                                                \
            map->flag = phyto_hash_flag_empty;                                                  \
            return false;                                                                       \
        }                                                                                       \
        bool first = true;                                                                      \
        phyto_string_t max_key = {0};                                                           \
        V max_value = (V){0};                                                                   \
        for (size_t i = 0; i < map->capacity; ++i) {                                            \
            Name##_entry_t* entry = &map->buffer[i];                                            \
            if (entry->state == Name##_entry_state_filled) {                                    \
                if (first) {                                                                    \
                    first = false;                                                              \
                    max_key = entry->key;                                                       \
                    max_value = entry->value;                                                   \
                } else if (phyto_string_compare(phyto_string_view(entry->key),                  \
                                                phyto_string_view(max_key)) > 0) {              \
                    max_key = entry->key;                                                       \
                    max_value = entry->value;                                                   \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
        if (out_key) {                                                                          \
            *out_key = max_key;                                                                 \
        }                                                                                       \
        if (out_value) {                                                                        \
            *out_value = max_value;                                                             \
        }                                                                                       \
        map->flag = phyto_hash_flag_ok;                                                         \
        return true;                                                                            \
    }                                                                                           \
    bool Name##_min(Name##_t* map, phyto_string_t* out_key, V* out_value) {                     \
        if (Name##_empty(map)) {                                                                \
            map->flag = phyto_hash_flag_empty;                                                  \
            return false;                                                                       \
        }                                                                                       \
        bool first = true;                                                                      \
        phyto_string_t min_key = {0};                                                           \
        V min_value = (V){0};                                                                   \
        for (size_t i = 0; i < map->capacity; ++i) {                                            \
            Name##_entry_t* entry = &map->buffer[i];                                            \
            if (entry->state == Name##_entry_state_filled) {                                    \
                if (first) {                                                                    \
                    first = false;                                                              \
                    min_key = entry->key;                                                       \
                    min_value = entry->value;                                                   \
                } else if (phyto_string_compare(phyto_string_view(entry->key),                  \
                                                phyto_string_view(min_key)) < 0) {              \
                    min_key = entry->key;                                                       \
                    min_value = entry->value;                                                   \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
        if (out_key) {                                                                          \
            *out_key = min_key;                                                                 \
        }                                                                                       \
        if (out_value) {                                                                        \
            *out_value = min_value;                                                             \
        }                                                                                       \
        map->flag = phyto_hash_flag_ok;                                                         \
        return true;                                                                            \
    }                                                                                           \
    V Name##_get(Name##_t* map, phyto_string_view_t key) {                                      \
        if (Name##_empty(map)) {                                                                \
            map->flag = phyto_hash_flag_empty;                                                  \
            return (V){0};                                                                      \
        }                                                                                       \
        Name##_entry_t* entry = Name##_impl_get_entry(map, key);                                \
        if (!entry) {                                                                           \
            map->flag = phyto_hash_flag_not_found;                                              \
            return (V){0};                                                                      \
        }                                                                                       \
        map->flag = phyto_hash_flag_ok;                                                         \
        return entry->value;                                                                    \
    }                                                                                           \
    V* Name##_get_ref(Name##_t* map, phyto_string_view_t key) {                                 \
        if (Name##_empty(map)) {                                                                \
            map->flag = phyto_hash_flag_empty;                                                  \
            return NULL;                                                                        \
        }                                                                                       \
        Name##_entry_t* entry = Name##_impl_get_entry(map, key);                                \
        if (!entry) {                                                                           \
            map->flag = phyto_hash_flag_not_found;                                              \
            return NULL;                                                                        \
        }                                                                                       \
        map->flag = phyto_hash_flag_ok;                                                         \
        return &entry->value;                                                                   \
    }                                                                                           \
    bool Name##_contains(Name##_t* map, phyto_string_view_t key) {                              \
        map->flag = phyto_hash_flag_ok;                                                         \
        return Name##_impl_get_entry(map, key) != NULL;                                         \
    }                                                                                           \
    bool Name##_empty(Name##_t* map) { return map->count == 0; }                                \
    bool Name##_full(Name##_t* map) {                                                           \
        return (double)map->capacity * map->load <= (double)map->count;                         \
    }                                                                                           \
    size_t Name##_count(Name##_t* map) { return map->count; }                                   \
    size_t Name##_capacity(Name##_t* map) { return map->capacity; }                             \
    double Name##_load(Name##_t* map) { return map->load; }                                     \
    phyto_hash_flag_t Name##_flag(Name##_t* map) { return map->flag; }                          \
    bool Name##_resize(Name##_t* map, size_t capacity) {                                        \
        map->flag = phyto_hash_flag_ok;                                                         \
        if (capacity == map->capacity) {                                                        \
            return true;                                                                        \
        }                                                                                       \
        if ((double)map->capacity > (double)capacity / map->load) {                             \
            return true;                                                                        \
        }                                                                                       \
        if ((double)capacity >= (double)UINTMAX_MAX * map->load) {                              \
            map->flag = phyto_hash_flag_error;                                                  \
            return false;                                                                       \
        }                                                                                       \
        size_t theoretical_size = Name##_impl_calculate_size(capacity);                         \
        if ((double)theoretical_size < (double)map->count / map->load) {                        \
            map->flag = phyto_hash_flag_overflow;                                               \
            return false;                                                                       \
        }                                                                                       \
        Name##_t* new_map = Name##_new(capacity, map->load, map->key_ops, map->value_ops);      \
        if (!new_map) {                                                                         \
            map->flag = phyto_hash_flag_alloc;                                                  \
            return false;                                                                       \
        }                                                                                       \
        for (size_t i = 0; i < map->capacity; ++i) {                                            \
            if (map->buffer[i].state == Name##_entry_state_filled) {                            \
                phyto_string_view_t key = phyto_string_view(map->buffer[i].key);                \
                V value = map->buffer[i].value;                                                 \
                if (!Name##_insert(new_map, key, value)) {                                      \
                    map->flag = new_map->flag;                                                  \
                    Name##_free(new_map);                                                       \
                    return false;                                                               \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
        if (map->count != new_map->count) {                                                     \
            map->flag = phyto_hash_flag_error;                                                  \
            Name##_free(new_map);                                                               \
            return false;                                                                       \
        }                                                                                       \
        Name##_entry_t* tmp_b = map->buffer;                                                    \
        map->buffer = new_map->buffer;                                                          \
        new_map->buffer = tmp_b;                                                                \
        size_t tmp_c = map->capacity;                                                           \
        map->capacity = new_map->capacity;                                                      \
        new_map->capacity = tmp_c;                                                              \
        new_map->value_ops = &((Name##_value_ops_t){0});                                        \
        Name##_free(new_map);                                                                   \
        return true;                                                                            \
    }                                                                                           \
    Name##_t* Name##_copy_of(Name##_t* map) {                                                   \
        Name##_t* result = Name##_new(map->capacity, map->load, map->key_ops, map->value_ops);  \
        if (!result) {                                                                          \
            map->flag = phyto_hash_flag_error;                                                  \
            return NULL;                                                                        \
        }                                                                                       \
        for (size_t i = 0; i < map->capacity; ++i) {                                            \
            Name##_entry_t* scan = &map->buffer[i];                                             \
            if (scan->state != Name##_entry_state_empty) {                                      \
                Name##_entry_t* target = &result->buffer[i];                                    \
                if (scan->state == Name##_entry_state_deleted) {                                \
                    target->state = Name##_entry_state_deleted;                                 \
                } else {                                                                        \
                    target->state = Name##_entry_state_filled;                                  \
                    target->key = phyto_string_copy(scan->key);                                 \
                    if (map->value_ops->copy) {                                                 \
                        target->value = map->value_ops->copy(scan->value);                      \
                    } else {                                                                    \
                        target->value = scan->value;                                            \
                    }                                                                           \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
        result->count = map->count;                                                             \
        map->flag = phyto_hash_flag_ok;                                                         \
        return result;                                                                          \
    }                                                                                           \
    bool Name##_equals(Name##_t* map1, Name##_t* map2) {                                        \
        map1->flag = phyto_hash_flag_ok;                                                        \
        map2->flag = phyto_hash_flag_ok;                                                        \
        if (map1->count != map2->count) {                                                       \
            return false;                                                                       \
        }                                                                                       \
        Name##_t* map_a = map1->capacity < map2->capacity ? map1 : map2;                        \
        Name##_t* map_b = map_a == map1 ? map2 : map1;                                          \
        for (size_t i = 0; i < map_a->capacity; ++i) {                                          \
            if (map_a->buffer[i].state == Name##_entry_state_filled) {                          \
                Name##_entry_t* entry_a = &map_a->buffer[i];                                    \
                Name##_entry_t* entry_b =                                                       \
                    Name##_impl_get_entry(map_b, phyto_string_view(entry_a->key));              \
                if (!entry_b) {                                                                 \
                    return false;                                                               \
                }                                                                               \
                if (map_a->value_ops->compare(entry_a->value, entry_b->value) != 0) {           \
                    return false;                                                               \
                }                                                                               \
            }                                                                                   \
        }                                                                                       \
        return true;                                                                            \
    }                                                                                           \
    static Name##_entry_t* Name##_impl_get_entry(Name##_t* map, phyto_string_view_t key) {      \
        size_t hash = map->key_ops->hash(key);                                                  \
        size_t pos = hash % map->capacity;                                                      \
        Name##_entry_t* target = &map->buffer[pos];                                             \
        while (target->state == Name##_entry_state_filled ||                                    \
               target->state == Name##_entry_state_deleted) {                                   \
            if (phyto_string_view_equal(key, phyto_string_view(target->key))) {                 \
                return target;                                                                  \
            }                                                                                   \
            pos++;                                                                              \
            target = &map->buffer[pos % map->capacity];                                         \
        }                                                                                       \
        return NULL;                                                                            \
    }                                                                                           \
    static size_t Name##_impl_calculate_size(size_t required) {                                 \
        const phyto_hash_prime_span_t prime_span = phyto_hash_prime_span();                     \
        const size_t count = prime_span.size;                                                   \
        if (prime_span.begin[count - 1] < required) {                                           \
            return required;                                                                    \
        }                                                                                       \
        size_t i = 0;                                                                           \
        while (prime_span.begin[i] < required) {                                                \
            i++;                                                                                \
        }                                                                                       \
        return prime_span.begin[i];                                                             \
    }

#endif  // PHYTO_HASH_HASH_H_
