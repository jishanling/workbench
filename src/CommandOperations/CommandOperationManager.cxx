/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#define __COMMAND_OPERATION_MANAGER_DEFINE__
#include "CommandOperationManager.h"
#undef __COMMAND_OPERATION_MANAGER_DEFINE__

#include "CommandClassCreate.h"
#include "CommandClassCreateEnum.h"
#include "CommandUnitTest.h"
#include "ProgramParameters.h"

using namespace caret;

/**
 * Get the command operation manager.
 *
 * return 
 *   Pointer to the command operation manager.
 */
CommandOperationManager* 
CommandOperationManager::getCommandOperationManager()
{
    if (singletonCommandOperationManager == NULL) {
        singletonCommandOperationManager = 
        new CommandOperationManager();
    }
    return singletonCommandOperationManager;
}

/**
 * Delete the command operation manager.
 */
void 
CommandOperationManager::deleteCommandOperationManager()
{
    if (singletonCommandOperationManager != NULL) {
        delete singletonCommandOperationManager;
        singletonCommandOperationManager = NULL;
    }
}

/**
 * Constructor.
 */
CommandOperationManager::CommandOperationManager()
{
    this->commandOperations.push_back(new CommandClassCreate());
    this->commandOperations.push_back(new CommandClassCreateEnum());
    this->commandOperations.push_back(new CommandUnitTest());
}

/**
 * Destructor.
 */
CommandOperationManager::~CommandOperationManager()
{
    uint64_t numberOfCommands = this->commandOperations.size();
    for (uint64_t i = 0; i < numberOfCommands; i++) {
        delete this->commandOperations[i];
        this->commandOperations[i] = NULL;
    }
    this->commandOperations.clear();
}

/**
 * Run a command.
 * 
 * @param parameters
 *    Reference to the command's parameters.
 * @throws CommandException
 *    If the command failed.
 */
void 
CommandOperationManager::runCommand(ProgramParameters& parameters) throw (CommandException)
{
    const uint64_t numberOfCommands = this->commandOperations.size();

    if (parameters.hasNext() == false) {
        for (uint64_t i = 0; i < numberOfCommands; i++) {
            CommandOperation* op = this->commandOperations[i];
            
            std::cout 
            << op->getCommandLineSwitch() 
            << " "
            << op->getOperationShortDescription()
            << std::endl;
        }
    
        return;
    }
    
    AString commandSwitch;
    try {
        commandSwitch = parameters.nextString("Command Name");
    
        CommandOperation* operation = NULL;
        
        for (uint64_t i = 0; i < numberOfCommands; i++) {
            if (this->commandOperations[i]->getCommandLineSwitch() == commandSwitch) {
                operation = this->commandOperations[i];
                break;
            }
        }
        
        if (operation == NULL) {
            throw CommandException("Command \"" + commandSwitch + "\" not found.");
        }
        
        operation->execute(parameters);
    }
    catch (ProgramParametersException& e) {
        throw CommandException(e);
    }
}

/**
 * Get the command operations.
 * 
 * @return
 *   A vector containing the command operations.
 * Do not modify the returned value.
 */
std::vector<CommandOperation*> 
CommandOperationManager::getCommandOperations()
{
    return this->commandOperations;
}



