all: interpret interpret_batch

interpret: 
	g++ ./interpret.cpp ./evaluate_mltl.cpp ./utils.cpp -o interpret

interpret_batch:
	g++ ./interpret_batch.cpp ./evaluate_mltl.cpp ./utils.cpp -o interpret_batch

clean:
	rm interpret interpret_batch
	