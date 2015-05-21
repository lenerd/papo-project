#ifndef NEURALNET
#define NEURALNET

#define E 2.7182818284

float sigmoid(float x);
float inverse_sigmoid(float x);

float random_value_01();
float random_value_0m(float max);
float random_value_mm(float min, float max);

struct neuralnet{
	int edges_count;

	int input_count;
	int hidden_layer_count;
	int neurons_per_hidden_layer;
	int output_count;

	float* input_edges;
	float* edges;
	float* output_edges;
};

struct neuralnet* allocate_neural_net(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count);
void initialize_neural_net(struct neuralnet* net);
struct neuralnet* create_neural_net(int input_count, int hidden_layer_count, int neurons_per_hidden_layer, int output_count);

void deallocate_neural_net(struct neuralnet* net);
void destroy_neural_net(struct neuralnet* net);

#endif
