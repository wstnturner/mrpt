/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2022, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */

#include <mrpt/comms/CSerialPort.h>

#include <iostream>
#include <memory>

using namespace mrpt::comms;
using namespace std;

int main()
{
	try
	{
		std::unique_ptr<CSerialPort> serPort;

		string serName;

		cout << "Serial port test application: Use it with a loopback serial "
				"port (pins 2-3 connected)"
			 << endl;

		cout << "Enter the serial port name (e.g. COM1, ttyS0, ttyUSB0): ";
		getline(cin, serName);

		cout << endl;
		cout << "Opening serial port...";
		serPort = std::make_unique<CSerialPort>(serName);
		cout << "OK" << endl;

		cout << "Setting timeouts...";
		serPort->setTimeouts(100, 1, 100, 1, 100);
		cout << "OK" << endl;

		cout << "Setting baud rate...";
		serPort->setConfig(500000);
		cout << "OK" << endl;

		for (int i = 0; i < 10; i++)
		{
			// Test write:
			cout << "Writing test data...";
			const char buf1[] = "Hello world!";
			size_t written = serPort->Write(buf1, sizeof(buf1));
			cout << written << " bytes written." << endl;

			// Read:
			cout << "Reading data...";
			char buf2[100];
			size_t nRead = serPort->Read(buf2, sizeof(buf2));
			cout << nRead << " bytes read: '";

			buf2[nRead] = 0;
			cout << buf2 << "'" << endl;
		}
	}
	catch (const std::exception& e)
	{
		cerr << e.what() << endl;
		return -1;
	}

	return 0;
}
