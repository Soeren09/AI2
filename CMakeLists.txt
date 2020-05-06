cmake_minimum_required(VERSION 3.16)
project(AI)

set(CMAKE_CXX_STANDARD 17)

add_executable(AI Game.cpp Game.h Player.cpp Player.h main.cpp Parameters.h PlayerBase.h PlayerRandom.h PlayerAC.h PlayerAC.cpp ActorCriticParameters.h Network.h)
#add_executable(AI Game.cpp Game.h Player.cpp Player.h Timing.cpp Parameters.h)

# Player.cpp Player.h