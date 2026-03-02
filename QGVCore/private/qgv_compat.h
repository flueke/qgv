#ifndef F8CE64B3_9540_431F_819D_34CE0A338A8D
#define F8CE64B3_9540_431F_819D_34CE0A338A8D

#include <graphviz_version.h>

#if defined(GRAPHVIZ_VERSION_MAJOR) && GRAPHVIZ_VERSION_MAJOR >= 13

// Compatible with the old agstrfree signature. Assumes the strings are html by
// default. No idea and no time to look for the info and motivation for the
// change.
inline int agstrfree(Agraph_t *g, const char *str)
{
    return agstrfree(g, str, true);
}
#endif

#endif /* F8CE64B3_9540_431F_819D_34CE0A338A8D */
