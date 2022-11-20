# Vernier-Arduino Firmware

This is the code for the 'firmware' version of the SparkFun Arduino-Vernier Shield.

While the library modules break out and abstract the connection and communications with the SparkFun
Hat this consolidates the principal devices into a generic data acquisition system for use with Vernier's probes.

The inspiration for this project comes from the consideration that my school has a big investment in Vernier Probes
that require a central data acquisition hub like **LabQuest** and/or **LabQUestMini** to collect and convert the data to something
we can use in our labs. Vernier has software that communicates nicely with these hubs but they do not support Linux (esp. the ubuntu variants) that our school increasingly relies upon. No shade on them, this is a limited market. I admire the fact that they are very open with their hardware design and whereever possible they have been extremely helpful with developing and releasing python code for their newer devices.

A particular focus has been to get the communication to work smoothly in a Jupyter Notebook which serves as the data acqusition tool for getting data from the sensors.  A big fan of electronic notebooks since I was an RSM at IBM eons ago, I have explored the use of tools that I can use in my physics classes. The advent of ipython and JupyterLab and JupyterNotebook as a venue for collecting, organizing, analyzing and (most importantly) reporting results has proven to be a valuable skill I can offer my students.

The folder [VernierShieldPythonComunication](../VernierShieldPythonCommunication) has some sample code for the intended use of a machine running python or even better an instance of a Jupyter environment for gathering and analyzing data.
