#pragma once

#include "pch.h"

#include <fann/doublefann.h>
#include <fann/fann_cpp.h>
#include <string>

class net {
	bool
		cascade_training,
		shuffle_data;
	int
		hindsight_level,
		foresight_level,
		training_epochs,
		hidden_layers,
		report_interval;
	float 
		desired_error,
		learning_momentum,
		learning_rate;
	pair_info *pi;
	std::string netname;
	FANN::training_algorithm_enum training_algorithm;
	FANN::neural_net ann;

	void create(void);
	void load(void);
	void save(void);
public:
	net(pair_info *pi);
	void reset(void);
	void train(void);
	void rebuild_database(int samples);
	double pulse(void);
};