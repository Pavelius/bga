#pragma once

void add_order(void* parent, void* object, fnevent apply);
// orderi* find_active_order(const void* parent);

void activate_order(void* parent);
void clear_orders(void* parent);
void update_orders();