#ifndef NEURALNET
#define NEURALNET

#define E 2.7182818284

float sigmoid(float x);
float inverseSigmoid(float x);

float randomValue01();
float randomValue0M(float max);
float randomValueMM(float min, float max);

struct neuralnet{
	int inputCount;
	int hiddenLayerCount;
	int neuronsPerHiddenLayer;
	int outputCount;

	float** inputEdges;
	float*** edges;
	float** outputEdges;
};

struct neuralnet* allocateNeuralNet(int inputCount, int hiddenLayerCount, int neuronsPerHiddenLayer, int outputCount);
void initializeNeuralNet(struct neuralnet* net);
struct neuralnet* createNeuralNet(int inputCount, int hiddenLayerCount, int neuronsPerHiddenLayer, int outputCount);

void deallocateNeuralNet(struct neuralnet* net);
void destroyNeuralNet(struct neuralnet* net);

#endif