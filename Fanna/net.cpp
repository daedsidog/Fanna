#pragma once

#include "pch.h"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>

#include "net.hpp"
#include "config.hpp"

static int g_console = 0;

void print_error(std::string errmsg) {
	std::cout << "ERROR: " << errmsg << std::endl;
	if(g_console == 1)
		system("pause");
}
void print_warning(std::string warnmsg) {
	std::cout << "WARNING: " << warnmsg << std::endl;
}

net::net(pair_info *pi) {
	g_console = pi->console;
	try {
		cascade_training = stoi(config::parse("cascade_training")) == 1 ? true : false;
		shuffle_data = stoi(config::parse("shuffle_data")) == 1 ? true : false;

		training_epochs = stoi(config::parse("training_epochs"));
		hidden_layers = stoi(config::parse("hidden_layers"));
		report_interval = stoi(config::parse("report_interval"));

		learning_momentum = stof(config::parse("learning_momentum"));
		learning_rate = stof(config::parse("learning_rate"));
		desired_error = stof(config::parse("desired_error"));

		hidden_layer_factor = stod(config::parse("hidden_layer_factor"));

		std::string training_algstr = config::parse("training_algorithm");
		if (training_algstr.find("INCREMENTAL") != training_algstr.npos)
			training_algorithm = FANN::TRAIN_INCREMENTAL;
		else if (training_algstr.find("BATCH") != training_algstr.npos)
			training_algorithm = FANN::TRAIN_BATCH;
		else if (training_algstr.find("QUICKPROP") != training_algstr.npos)
			training_algorithm = FANN::TRAIN_QUICKPROP;
		else if (training_algstr.find("SAPROP") != training_algstr.npos)
			training_algorithm = FANN::TRAIN_SARPROP;
		else training_algorithm = FANN::TRAIN_RPROP;
	}
	catch (std::exception ex) {
		print_error("Could not read from config file. Is it located in the working directory?");
		return;
	}
	this->pi = pi;
	hindsight_level = pi->hindsight;
	std::stringstream namestream;
	namestream << pi->pair << pi->interval;
	netname = namestream.str();
	std::stringstream netss;
	netss << netname << "\\" << netname << ".net";
	if (std::filesystem::exists(netss.str()))
		load();
	else create();
	ann.set_learning_momentum(learning_momentum);
	ann.set_learning_rate(learning_rate);
}
int net::reset(void) {
	std::cout << "Resetting " << netname << "..." << std::endl;
	std::filesystem::remove((std::stringstream() << netname << "\\" << netname << ".net").str());
	ann.destroy();
	create();
	save();
	return 1;
}
int net::rebuild_database(void) {
	if (hindsight_level >= pi->length) {
		print_error("Hindsight level is too large for the available chart history.");
		return 0;
	}
	std::stringstream dbname;
	dbname << netname << "\\" << netname << ".dat";
	std::ofstream database(dbname.str());
	int samples = 0;
	std::vector<double> inputs, outputs;
	for (int i = 0; i < pi->length - hindsight_level; ++i) {
		std::cout << "\rRebuilding database (" << i + 1 << "/" << pi->length - hindsight_level << ")...";
		bool price_met = false;
		double
			upper_bound = pi->closing_price[i + 1] + pi->offset,
			lower_bound = pi->closing_price[i + 1] - pi->offset;
		for (int j = i; j > 0 && !price_met; --j) {
			if (pi->max_price[j] > upper_bound) {
				if (pi->min_price[j] > lower_bound) {
					outputs.push_back(1.0);
					price_met = true;
				}
			}
			else {
				if (pi->min_price[j] < lower_bound) {
					outputs.push_back(0.0);
					price_met = true;
				}
			}
		}
		if (price_met) {
			++samples;
			for (int j = 0; j < hindsight_level; ++j) {
				int idx = j + i + 1;
				inputs.push_back(pi->opening_price[idx]);
				inputs.push_back(pi->closing_price[idx]);
				inputs.push_back(pi->max_price[idx]);
				inputs.push_back(pi->min_price[idx]);
				inputs.push_back(pi->volume[idx]);
			}
		}
	}
	database << samples << " " << hindsight_level * 5 << " 1" << std::endl;
	for (int i = 0; i < samples; ++i) {
		for (int j = 0; j < hindsight_level * 5; ++j)
			database << inputs.at(i * hindsight_level * 5 + j) << " ";
		database << std::endl << outputs.at(i) << " " << std::endl;
	}
	database.close();
	std::cout << std::endl;
	return samples == 0 ? 0 : 1;
}
int net::train(void){
	if (!std::filesystem::exists((std::stringstream() << netname << "\\" << netname << ".dat").str())) {
		print_error((std::stringstream() << "No training database file for " << netname << " detected. Have you built one?").str());
		return 0;
	}
	FANN::training_data data;
	data.read_train_from_file((std::stringstream() << netname << "\\" << netname << ".dat").str());
	if (data.num_input_train_data() != ann.get_num_input()) {
		print_error("Mismatch between training file and ANN. Rebuild the training database.");
		return 0;
	}
	ann.set_training_algorithm(training_algorithm);
	if (shuffle_data)
		data.shuffle_train_data();
	ann.set_scaling_params(data, 0.0f, 1.0f, 0.0f, 1.0f);
	ann.scale_train(data);
	if (!cascade_training) 
		ann.train_on_data(data, training_epochs, report_interval, desired_error);
	else ann.cascadetrain_on_data(data, INT_MAX, 1, desired_error);
	data.destroy_train();
	save();
	return 1;
}
double net::pulse(void){
	std::vector<double> inputs;
	for (int i = 0; i < hindsight_level; ++i) {
		inputs.push_back(pi->opening_price[i]);
		inputs.push_back(pi->closing_price[i]);
		inputs.push_back(pi->max_price[i]);
		inputs.push_back(pi->min_price[i]);
		inputs.push_back(pi->volume[i]);
	}
	double* inputs_array = inputs.data();
	ann.scale_input(inputs_array);
	double *outputs = ann.run(inputs_array);
	return outputs[0];
}

void net::load(void) {
	std::cout << "Loading " << netname << "..." << std::endl;
	ann.create_from_file((std::stringstream() << netname << "\\" << netname << ".net").str());
	if ((ann.get_num_input() != hindsight_level * 5)
		|| ((ann.get_num_layers() != hidden_layers + 2) && !cascade_training)
		|| ((ann.get_total_neurons() - (ann.get_num_layers() - 1) != 1 + hindsight_level * 5 + int(round(double(double(hindsight_level) * 5) * hidden_layer_factor)) * hidden_layers) && !cascade_training)) {
		print_warning("Mismatch between ANN and configuration.");
		reset();
	}
}
void net::create(void) {
	std::cout << "Creating " << netname << "..." << std::endl;
	try {
		if (!std::filesystem::exists(netname))
			std::filesystem::create_directory(netname);
	}
	catch (std::exception ex) {
		print_error("Could not create ANN directory. Make sure you run the script as administrator.");
		return;
	}
	if (!cascade_training) {
		std::vector<unsigned int> layers;
		layers.push_back(hindsight_level * 5);
		for (int i = 0; i < hidden_layers; ++i)
			layers.push_back(int(round(double(double(hindsight_level) * 5) * hidden_layer_factor)));
		layers.push_back(1);
		ann.create_standard_array(2 + hidden_layers, layers.data());
		ann.set_activation_function_hidden(FANN::ELLIOT_SYMMETRIC);
		ann.set_activation_function_output(FANN::ELLIOT);
	}
	else ann.create_shortcut(2, hindsight_level * 5, 1);
	ann.randomize_weights(-1.0f, 1.0f);
}
void net::save(void) {
	std::cout << "Saving network..." << std::endl;
	ann.save((std::stringstream() << netname << "\\" << netname << ".net").str());
}