#include "CommandSolver.hpp"

using namespace commandsolver;
using namespace nlohmann;

void CreateCommand::execute(std::shared_ptr<simplepubsub::IPublisher> ptr)
{
    ptr->publish("CreateFile", json{{"id", data.id},
                                    {"redius", data.redius},
                                    {"height", data.height}}
                                   .dump());
}