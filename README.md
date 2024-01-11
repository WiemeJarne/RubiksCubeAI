Boost library version 1.67.0 is required for the Q-learning project.

- Download boost_1_67_0.zip here: https://boostorg.jfrog.io/artifactory/main/release/1.67.0/source/
- Unzip the folder and go in it.
- Open x86 Native Tools Command Prompt for your visual studio version.
- Use next command: bootstrap.bat
- Wait until the command is done executing.
- Use the next command: .\b2 address-model=64 --with-serialization --build-type=complete
- Now the files are generated for serialization.
- Change C:\boost_1_67_0 in the aditional include directories in the properties of the Q-learning project to the location of your boost_1_67_0 folder.
