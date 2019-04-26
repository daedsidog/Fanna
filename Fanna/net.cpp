#pragma once

#include "pch.h"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>

#include "net.hpp"
#include "config.hpp"


net::net(pair_info *pi) {
	try {
		cascade_training = stoi(config::parse("cascade_training")) == 1 ? true : false;
		shuffle_data = stoi(config::parse("shuffle_data")) == 1 ? true : false;
		training_epochs = stoi(config::parse("training_epochs"));
		hidden_layers = stoi(config::parse("hidden_layers"));
		report_interval = stoi(config::parse("report_interval"));
		cascade_addend = stoi(config::parse("cascade_addend"));

		learning_momentum = stof(config::parse("learning_momentum"));
		learning_rate = stof(config::parse("learning_rate"));
		desired_error = stof(config::parse("desired_error"));

		std::string training_algstr = config::parse("training_algorithm");
		if (training_algstr.find("INCREMENTAL") != training_algstr.npos)
			training_algorithm = FANN::TRAIN_INCREMENTAL;
		else if (training_algstr.find("BATCH") != training_algstr.npos)
			training_algorithm = FANN::TRAIN_BATCH;
		else if (training_algstr.find("QUICKPROP") != training_algstr.npos)
			training_algorithm = FANN::TRAIN_QUICKPROP;
		else training_algorithm = FANN::TRAIN_RPROP;
	}
	catch (std::exception ex) {
		std::cout << "ERROR: Could not read from config file. Is it located in the working directory?" << std::endl;
	}
	this->pi = pi;
	hindsight_level = pi->hindsight;
	foresight_level = pi->foresight;
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
void net::load(void) {
	std::cout << "Loading " << netname << "..." << std::endl;
	ann.create_from_file((std::stringstream() << netname << "\\" << netname << ".net").str());
}
void net::create(void) {
	std::cout << "Creating " << netname << "..." << std::endl;
	try {
		if (!std::filesystem::exists(netname))
			std::filesystem::create_directory(netname);
	}
	catch (std::exception ex) {
		std::cout << "ERROR: Could not create ANN directory. Make sure you run the script as administrator." << std::endl;
	}
	if (!cascade_training) {
		std::vector<unsigned int> layers;
		layers.push_back(hindsight_level * 5);
		for (int i = 0; i < hidden_layers; ++i)
			layers.push_back(hindsight_level * 5);
		layers.push_back(1);
		ann.create_standard_array(2 + hidden_layers, layers.data());
		ann.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC);
	}
	else ann.create_shortcut(2, hindsight_level * 5, 1);
	ann.randomize_weights(-1.0f, 1.0f);
}
void net::reset(void) {
	std::cout << "Resetting " << netname << "..." << std::endl;
	std::filesystem::remove((std::stringstream() << netname << "\\" << netname << ".net").str());
	ann.destroy();
	create();
	save();
}
void net::save(void) {
	std::cout << "Saving network..." << std::endl;
	ann.save((std::stringstream() << netname << "\\" << netname << ".net").str());
}
void net::rebuild_database(int samples) {
	if (pi->length - (hindsight_level + foresight_level) >= samples)  {
		std::stringstream dbname;
		dbname << netname << "\\" << netname << ".dat";
		std::ofstream database(dbname.str());
		database << samples << " " << hindsight_level * 5 << " 1" << std::endl;
		for (int i = 0; i < samples; ++i) {
			std::cout << "\rRebuilding database (" << i + 1 << "/" << samples << ")...";
			double avg = 0.0;
			for (int j = 0; j < hindsight_level; ++j) {
				int idx = j + i + 1 + foresight_level;
				avg += pi->max_price[idx] - pi->min_price[idx];
				database << pi->opening_price[idx] << " ";
				database << pi->closing_price[idx] << " ";
				database << pi->max_price[idx] << " ";
				database << pi->min_price[idx] << " ";
				database << pi->volume[idx] << " ";
			}
			avg /= double(hindsight_level);
			database << std::endl;
			bool price_met = false;
			double
				upper_bound = pi->closing_price[i + 1 + foresight_level] + avg,
				lower_bound = pi->closing_price[i + 1 + foresight_level] - avg;
			for (int j = foresight_level; j > 0 && !price_met; --j) {
				int idx = j + i;
				if (pi->max_price[idx] >= upper_bound) {
					if (pi->min_price[idx] > lower_bound) {
						database << "1.0" << std::endl;
						price_met = true;
					}
					else {
						database << "0.5" << std::endl;
						price_met = true;
					}
				}
				else {
					if (pi->min_price[idx] <= lower_bound) {
						database << "0.0" << std::endl;
						price_met = true;
					}
					else {
						database << "0.5" << std::endl;
						price_met = true;
					}
				}
			}
			if (!price_met)
				database << "0.5" << std::endl;
		}
		database.close();
		std::cout << std::endl;
	}
	else std::cout << "ERROR: Amount of samples too large for the available pair data." << std::endl;
}
void net::train(void){
	if (!std::filesystem::exists((std::stringstream() << netname << "\\" << netname << ".dat").str()))
		std::cout << "ERROR: No training database file for " << netname << " detected. Have you built one?" << std::endl;
	FANN::training_data data;
	data.read_train_from_file((std::stringstream() << netname << "\\" << netname << ".dat").str());
	ann.set_training_algorithm(training_algorithm);
	if (shuffle_data)
		data.shuffle_train_data();
	if(!cascade_training)
		ann.train_on_data(data, training_epochs, report_interval, desired_error);
	else ann.cascadetrain_on_data(data, INT_MAX, cascade_addend, desired_error);
	data.destroy_train();
	save();
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
	double *outputs = ann.run(inputs.data());
	return outputs[0];
}