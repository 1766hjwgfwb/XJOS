#ifndef XJOS_DEBUG_H
#define XJOS_DEBUG_H



void debug(char *file, int line, const char *fmt, ...);


#define DEBUGK(fmt, args...) debug(__BASE_FILE__, __LINE__, fmt, ##args);





#endif /* XJOS_DEBUG_H */