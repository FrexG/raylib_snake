CC := gcc
CFLAGS := -Wall -Wextra
LIBS := -lraylib -lm -lpthread -ldl
SRCS := snake.c
OBJS := snake.o
MAIN := snake

${MAIN}:${OBJS}
	${CC} ${CFLAGS} -o ${MAIN} ${OBJS} ${LIBS}

${OBJS}:${SRCS}
	${CC} ${CFLAGS} -c ${SRCS} -o ${OBJS} ${LIBS}

clean:
	rm -f ${MAIN} ${OBJS}
