#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>

#include "net.hpp"
#include "config.hpp"


net::net(pair_info *pi) {
	cascade_training = stoi(config::parse("cascade_training")) == 1 ? true : false;
	shuffle_data = stoi(config::parse("shuffle_data")) == 1 ? true : false;

	hindsight_level = stoi(config::parse("hindsight_level"));
	training_epochs = stoi(config::parse("training_epochs"));
	hidden_layers = stoi(config::parse("hidden_layers"));
	report_interval = stoi(config::parse("report_interval"));

	learning_momentum = stod(config::parse("learning_momentum"));
	learning_rate = stod(config::parse("learning_rate"));
	desired_error = stod(config::parse("desired_error"));

	std::string training_algstr = config::parse("training_algorithm");
	if (training_algstr.find("INCREMENTAL") != training_algstr.npos)
		training_algorithm = FANN::TRAIN_INCREMENTAL;
	else if (training_algstr.find("BATCH") != training_algstr.npos)
		training_algorithm = FANN::TRAIN_BATCH;
	else if (training_algstr.find("QUICKPROP") != training_algstr.npos)
		training_algorithm = FANN::TRAIN_QUICKPROP;
	else training_algorithm = FANN::TRAIN_RPROP;
	std::stringstream namestream;
	namestream << pi->pair << pi->interval;
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
void net::rebuild_database(void) {
	int samples_detected = 0;
	for (auto& file : std::filesystem::directory_iterator("samples")) 
		if (file.path().filename().string().find("(1)") != std::string::npos && file.path().extension().string() == ".png")
			++samples_detected;
	int progress = 0;
	for (int d = 0; d < 3; ++d) {
		std::ofstream datafile((std::stringstream() << "ann\\" << netname[d] << ".dat").str());
		datafile << (samples_detected * 4 * 2) << " " << image_height * image_width << " " << image_height * image_width << std::endl;
		std::cout << "\rRebuilding database (" << progress << "/" << samples_detected * 4 * 2 * 3 << ")...";
		for (auto& file : std::filesystem::directory_iterator("samples")) {
			if (file.path().filename().string().find("(1)") != std::string::npos && file.path().extension().string() == ".png") {
				for (int f = 0; f < 2; ++f) {
					for (int n = 0; n < 4; ++n) {
						std::string* interpolation;
						switch (n) {
						case 0:
							interpolation = new std::string[4]{ "(1)", "(2)", "(3)", "(4)" };
							break;
						case 1:
							interpolation = new std::string[4]{ "(2)", "(1)", "(4)", "(3)" };
							break;
						case 2:
							interpolation = new std::string[4]{ "(3)", "(4)", "(2)", "(1)" };
							break;
						case 3:
							interpolation = new std::string[4]{ "(4)", "(3)", "(1)", "(2)" };
						}
						std::stringstream inputs_ss;
						inputs_ss << "samples\\" << file.path().stem().string().substr(0, file.path().stem().string().find(" ")) << " " << interpolation[0] << file.path().extension().string();
						std::vector<float> inputs;
						if (f == 0)
							inputs = imgmanip::image_to_scalar_array(inputs_ss.str());
						else
							inputs = imgmanip::image_to_scalar_flipped_array(inputs_ss.str());

						std::vector<float> outputs;
						std::stringstream outputs_ss;
						outputs_ss << "samples\\" << file.path().stem().string().substr(0, file.path().stem().string().find(" ")) << " " << interpolation[d + 1] << file.path().extension().string();
						std::vector<float> array;
						if (f == 0)
							outputs = imgmanip::image_to_scalar_array(outputs_ss.str());
						else
							outputs = imgmanip::image_to_scalar_flipped_array(outputs_ss.str());

						for (auto& input : inputs)
							datafile << input << " ";
						datafile << std::endl;
						for (auto& output : outputs)
							datafile << output << " ";
						datafile << std::endl;
						delete[] interpolation;
						++progress;
						std::cout << "\rRebuilding database (" << progress << "/" << samples_detected * 4 * 2 * 3 << ")...";
					}
				}
			}
		}
		datafile.close();
	}
	std::cout << std::endl;
}
void net::train(void) {
	std::thread *thread_object[3];
	for (int i = 0; i < 3; ++i) {
		if (!std::filesystem::exists((std::stringstream() << "ann\\" << netname[i] << ".dat").str()))
			std::cout << "ERROR: No training database file for " << netname[i] << " detected. Have you built one?" << std::endl;
	}
	for (int i = 0; i < 3; ++i) 
			thread_object[i] = new std::thread(static_cast<void (net::*)(int)>(&net::train), this, i);
	for (int i = 0; i < 3; ++i)
		thread_object[i]->join();
}
void net::train(int netidx){
	FANN::training_data data;
	data.read_train_from_file((std::stringstream() << "ann\\" << netname[netidx] << ".dat").str());
	ann[netidx].set_training_algorithm(training_algorithm);
	if (shuffle_data)
		data.shuffle_train_data();
	if(!cascade_training)
		ann[netidx].train_on_data(data, training_epochs, report_interval, desired_error);
	else ann[netidx].cascadetrain_on_data(data, INT_MAX, 1, desired_error);
	data.destroy_train();
}
void net::test(void){
	for (auto& file : std::filesystem::directory_iterator("tests")) {
		if (file.path().extension().string() == ".png") {
			std::cout << "Testing " << file.path().filename().string() << "..." << std::endl;
			std::vector<float> inputs = imgmanip::image_to_scalar_array(file.path().string());
			float *segmented_outputs[3];
			for (int i = 0; i < 3; ++i)
				segmented_outputs[i] = ann[i].run(inputs.data());
			std::vector<float> outputs;
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < image_height * image_width; ++j) {
					outputs.push_back(segmented_outputs[i][j]);
				}
			}
			imgmanip::present_panes(inputs.data(), outputs.data());
		}
	}
}