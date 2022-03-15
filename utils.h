#pragma once
#define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__);
