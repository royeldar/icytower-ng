#pragma once

#include <stdbool.h>
#include <stddef.h>

struct hashmap;
struct hashmap_entry;
struct hashmap_iter;

const char *hashmap_entry_get_str(const struct hashmap_entry *entry);
void **hashmap_entry_get_value_ptr(struct hashmap_entry *entry);
struct hashmap *hashmap_create(size_t initial_size);
void hashmap_destroy(struct hashmap *map, void (*callback)(struct hashmap_entry *entry, void *extra), void *extra);
struct hashmap_entry *hashmap_get(const struct hashmap *map, const char *str);
struct hashmap_entry *hashmap_get_next(const struct hashmap *map, const struct hashmap_entry *entry);
bool hashmap_add(struct hashmap *map, const char *str, void *ptr);
bool hashmap_remove(struct hashmap *map, const char *str);
struct hashmap_iter *hashmap_iter_create(const struct hashmap *map);
void hashmap_iter_reset(struct hashmap_iter *iter);
struct hashmap_entry *hashmap_iter_next(struct hashmap_iter *iter);
void hashmap_iter_destroy(struct hashmap_iter *iter);
