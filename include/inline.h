#ifndef INLINE_H
#define INLINE_H

// Helper define to not always have to apply the attribute to make sure a function is actually inlined
#define INLINE_FUNC __attribute__((always_inline)) inline

#endif
