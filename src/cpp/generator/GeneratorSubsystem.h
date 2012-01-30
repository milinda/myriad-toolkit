/*
 * Copyright 2010-2011 DIMA Research Group, TU Berlin
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * @author: Alexander Alexandrov <alexander.alexandrov@tu-berlin.de>
 */

#ifndef GENERATORSUBSYSTEM_H_
#define GENERATORSUBSYSTEM_H_

#include "generator/GeneratorPool.h"

#include <string>
#include <Poco/NotificationCenter.h>
#include "Poco/ThreadPool.h"
#include <Poco/Util/Application.h>
#include <Poco/Util/Subsystem.h>

using namespace std;
using namespace Poco;
using namespace Poco::Util;

namespace Myriad {

class GeneratorSubsystem: public Subsystem
{
	friend class GeneratorErrorHandler;
	friend class ThreadExecutor;

public:

	GeneratorSubsystem(NotificationCenter& notificationCenter, const vector<bool>& executeStages) :
		_notificationCenter(notificationCenter),
		_executeStages(executeStages),
		_config(_generatorPool),
		_threadPool("GeneratorPool", 4, 32),
		_initialized(false),
		_logger(Logger::get("generator.driver")),
		_ui(Logger::get("ui"))
	{
	}

	~GeneratorSubsystem()
	{
	}

	void start();

protected:

	const char* name() const
	{
		return "Generator Subsystem";
	}

	void initialize(Application&);

	void uninitialize();

	void registerGenerators();

private:
	/**
	 * Prepares all generators for the next stage.
	 *
	 * @param stage
	 */
	void prepareStage(RecordGenerator::Stage stage);

	/**
	 * Cleans up all generators upon execution of the current stage.
	 *
	 * @param stage
	 */
	void cleanupStage(RecordGenerator::Stage stage);

	/**
	 * Helper method. Adds a single generator to the generator pool.
	 *
	 * @param generator
	 */
	template<class T> void registerGenerator(const string& name);

	/**
	 * A reference to the application wide notification center.
	 */
	NotificationCenter& _notificationCenter;

	/**
	 * A bitmap of stages that need to be executed.
	 */
	const vector<bool>& _executeStages;

	/**
	 * A pool for the registered generators.
	 */
	GeneratorPool _generatorPool;

	/**
	 * An application wide generator config instance.
	 */
	GeneratorConfig _config;

	/**
	 * A pool for the runnable generator task threads.
	 */
	ThreadPool _threadPool;

	/**
	 * A flag indicating that the initialize() method has already been invoked.
	 */
	bool _initialized;

	/**
	 * Generator subsystem logger.
	 */
	Logger& _logger;

	/**
	 * User interface (normally stdout) logger.
	 */
	Logger& _ui;
};

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
// method definitions
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

template<class T> void GeneratorSubsystem::registerGenerator(const string& name)
{
	_generatorPool.set(new T(name, _config, _notificationCenter));
}

} // namespace Myriad

#endif /* GENERATORSUBSYSTEM_H_ */
