/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2022, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */

#include "nav-precomp.h"  // Precomp header
//
#include <mrpt/core/initializer.h>
#include <mrpt/nav.h>
#include <mrpt/serialization/CSerializable.h>

using namespace mrpt;
using namespace mrpt::nav;
using namespace std;

MRPT_INITIALIZER(registerAllNavigationClasses)
{
#if !defined(DISABLE_MRPT_AUTO_CLASS_REGISTRATION)
	// PTGs:
	registerClass(CLASS_ID(CParameterizedTrajectoryGenerator));
	registerClass(CLASS_ID(CPTG_DiffDrive_C));
	registerClass(CLASS_ID(CPTG_DiffDrive_alpha));
	registerClass(CLASS_ID(CPTG_DiffDrive_CCS));
	registerClass(CLASS_ID(CPTG_DiffDrive_CC));
	registerClass(CLASS_ID(CPTG_DiffDrive_CS));
	registerClass(CLASS_ID(CPTG_Holo_Blend));

	// Logs:
	registerClass(CLASS_ID(CLogFileRecord));
	registerClass(CLASS_ID(CLogFileRecord_ND));
	registerClass(CLASS_ID(CLogFileRecord_VFF));
	registerClass(CLASS_ID(CLogFileRecord_FullEval));

	// Holo methods:
	registerClass(CLASS_ID(CHolonomicVFF));
	registerClass(CLASS_ID(CHolonomicND));
	registerClass(CLASS_ID(CHolonomicFullEval));

	// Motion choosers:
	registerClass(CLASS_ID(CMultiObjectiveMotionOptimizerBase));
	registerClass(CLASS_ID(CMultiObjMotionOpt_Scalarization));
#endif
}
