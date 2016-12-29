cc=clang

main.o: main.c
	cc -o main.o -c main.c

tokenizer.o: tokenizer.c
	cc -o tokenizer.o -c tokenizer.c

queuestruct.o: queuestruct.c
	cc -o queuestruct.o -c queuestruct.c

stackstruct.o: stackstruct.c
	cc -o stackstruct.o -c stackstruct.c

functions.o: functions.c
	cc -o functions.o -c functions.c

shell: main.o tokenizer.o functions.o stackstruct.o queuestruct.o
	cc -o shell main.o tokenizer.o functions.o stackstruct.o queuestruct.o -I.

clean:
	rm main.o tokenizer.o functions.o stackstruct.o queuestruct.o