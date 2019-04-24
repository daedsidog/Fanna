#pragma once

#include "pch.h"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>

#include "net.hpp"
#include "config.hpp"


net::net(pair_info pi) {
	cascade_training = stoi(config::parse("cascade_training")) == 1 ? true : false;
	shuffle_data = stoi(config::parse("shuffle_data")) == 1 ? true : false;

	hindsight_level = stoi(config::parse("hindsight_level"));
	training_epochs = stoi(config::parse("training_epochs"));
	hidden_layers = stoi(config::parse("hidden_layers"));
	report_interval = stoi(config::parse("report_interval"));

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
	std::stringstream namestream;
	namestream << pi.pair << pi.interval;
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
}
void net::save(void) {
	std::cout << "Saving network..." << std::endl;
	ann.save((std::stringstream() << netname << "\\" << netname << ".net").str());
}
void net::rebuild_database(int samples) {
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
	else ann.cascadetrain_on_data(data, INT_MAX, 1, desired_error);
	data.destroy_train();
}
double net::pulse(void){
	return 0.0;
}