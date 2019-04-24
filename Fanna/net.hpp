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
		training_epochs,
		hidden_layers,
		report_interval;
	double 
		desired_error,
		learning_momentum,
		learning_rate;
	pair_info pi;
	std::string netname;
	FANN::training_algorithm_enum training_algorithm;
	FANN::neural_net ann;

	void create(void);
	void load(void);
public:
	net(pair_info *pi);
	void reset(void);
	void train(void);
	void train(void);
	void rebuild_database(void);
	void test(void);
	void save(void);
};