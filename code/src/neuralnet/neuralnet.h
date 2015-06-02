#ifndef NEURALNET_H
#define NEURALNET_H

struct neuralnet{
	int edges_count;

	int input_count;
	int hidden_layer_count;
	int neurons_per_hidden_layer;
	int output_count;

	float* edges;
};

//Returns how many edges a neural net has, based on its preferences.
int edge_count(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count);

struct neuralnet* allocate_neural_net(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count);

void initialize_neural_net_random(struct neuralnet* net);
void initialize_neural_net_buffer(struct neuralnet* net, float* buffer);
void initialize_neural_net_data(struct neuralnet* net, char* filepath);

struct neuralnet* create_neural_net_random(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count);
struct neuralnet* create_neural_net_buffer(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count, float* edges);
struct neuralnet* create_neural_net_data(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count, char* filepath);

void deallocate_neural_net(struct neuralnet* net);

//Use this to get rid of a neural net.
void destroy_neural_net(struct neuralnet* net);

//Calculates the output of the neural net based on input.
void calculate_output(const struct neuralnet* net, float* input, float* output);

//Print the edge weights to console. TH stands for threashold, EWs are the edge weights. 
//Each row stands for one receiving neuron with it's TH and receiving EWs.
void print_neural_net(const struct neuralnet* net);

#endif /* NEURALNET_H */
