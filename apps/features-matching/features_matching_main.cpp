/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2022, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */

#include <mrpt/containers/copy_container_typecasting.h>
#include <mrpt/gui/CDisplayWindow.h>
#include <mrpt/gui/CDisplayWindowPlots.h>
#include <mrpt/io/CMemoryStream.h>
#include <mrpt/math/data_utils.h>
#include <mrpt/obs/stock_observations.h>
#include <mrpt/serialization/CArchive.h>
#include <mrpt/vision/CFeatureExtraction.h>

#include <thread>

using namespace mrpt::math;
using namespace mrpt::gui;
using namespace mrpt::img;
using namespace mrpt::system;
using namespace mrpt::serialization;
using namespace mrpt::io;
using namespace mrpt::vision;
using namespace mrpt;
using namespace std;

static string file1, file2;

bool DemoFeatures()
{
	// Ask for the pair of images:
	cout << "Note: On all questions, [enter] means taking the default value"
		 << endl
		 << endl;

	if (file1.empty())
	{
		cout << "Enter path to image #1 [sample_image1]: ";
		std::getline(cin, file1);
	}
	else
		cout << "Image #1: " << file1 << endl;

	if (file2.empty())
	{
		cout << "Enter path to image #2 [sample_image2]: ";
		std::getline(cin, file2);
	}
	else
		cout << "Image #2: " << file2 << endl;

	// --------------------------------------
	// Ask the user for the feature method
	// --------------------------------------
	mrpt::vision::CFeatureExtraction fext;

	cout << endl
		 << "Detectors:\n"
			"0: KLT\n"
			"1: Harris\n"
			"2: BCD\n"
			"3: SIFT\n"
			"4: SURF\n"
			"6: FAST\n"
			"10: ORB\n"
			"11: AMAZE\n"
			"12: LSD\n";

	cout << endl << "Select the number for the desired method [0: KLT]:";

	string sel_method;
	std::getline(cin, sel_method);

	if (sel_method.empty()) fext.options.featsType = featKLT;
	else
		fext.options.featsType = TKeyPointMethod(atoi(sel_method.c_str()));

	// Compute descriptors:
	auto desc_to_compute = TDescriptorType(-1);

	if (fext.options.featsType != featSIFT &&
		fext.options.featsType != featSURF)
	{
		cout << R"(
Descriptors:
-1: None
0: Patch correlation
1: SIFT
2: SURF
4: Intensity-domain spin image descriptors
8: Polar image descriptor
16: Log-Polar image descriptor
32: ORB
64: BLD
128: LATCH)";

		cout << endl << "Select the number for the desired method [1: SIFT]:";

		string desc_method;
		std::getline(cin, desc_method);

		if (desc_method.empty()) desc_to_compute = descSIFT;
		else
			desc_to_compute = TDescriptorType(atoi(desc_method.c_str()));
	}

	// Max. num of features:
	cout << endl << "Maximum number of features [150 (default), 0: Infinite]:";
	string sel_num_feats;
	std::getline(cin, sel_num_feats);

	// Max # of features
	const size_t nFeats =
		sel_num_feats.empty() ? int(150) : int(::atoi(sel_num_feats.c_str()));

	CImage img1, img2;

	if (!file1.empty())
	{
		if (!img1.loadFromFile(file1))
			THROW_EXCEPTION_FMT("Error loading file: %s", file1.c_str());
	}
	else
		mrpt::obs::stock_observations::exampleImage(img1, 0);

	if (!file2.empty())
	{
		if (!img2.loadFromFile(file2))
			THROW_EXCEPTION_FMT("Error loading file: %s", file2.c_str());
	}
	else
		mrpt::obs::stock_observations::exampleImage(img2, 1);

	// Only extract patchs if we are using it: descAny means take the patch:
	if (desc_to_compute != descAny)
		fext.options.patchSize = 0;	 // Do not extract patch:

	CFeatureList feats1, feats2;

	CTicTac tictac;

	cout << "Detecting features in image1...";
	tictac.Tic();
	fext.detectFeatures(img1, feats1, 0, nFeats);
	cout << tictac.Tac() * 1000 << " ms (" << feats1.size() << " features)\n";

	cout << "Detecting features in image2...";
	tictac.Tic();
	fext.detectFeatures(img2, feats2, 0, nFeats);
	cout << tictac.Tac() * 1000 << " ms (" << feats2.size() << " features)\n";

	if (desc_to_compute != TDescriptorType(-1) && desc_to_compute != descAny)
	{
		const size_t N_TIMES = 1;
		// const size_t N_TIMES = 10;

		cout << "Extracting descriptors from image 1...";
		tictac.Tic();
		for (size_t timer_loop = 0; timer_loop < N_TIMES; timer_loop++)
			fext.computeDescriptors(img1, feats1, desc_to_compute);
		cout << tictac.Tac() * 1000.0 / N_TIMES << " ms" << endl;

		cout << "Extracting descriptors from image 2...";
		tictac.Tic();
		for (size_t timer_loop = 0; timer_loop < N_TIMES; timer_loop++)
			fext.computeDescriptors(img2, feats2, desc_to_compute);
		cout << tictac.Tac() * 1000.0 / N_TIMES << " ms" << endl;
	}

	CDisplayWindow win1("Image1"), win2("Image2");

	win1.setPos(10, 10);
	win1.showImageAndPoints(img1, feats1, TColor::blue());

	win2.setPos(20 + img1.getWidth(), 10);
	win2.showImageAndPoints(img2, feats2, TColor::blue());

	cout << "Showing all the features" << endl;
	cout << "Press any key on windows 1 or the console to continue..." << endl;
	win1.waitForKey();

	CDisplayWindowPlots winPlots("Distance between descriptors");
	winPlots.setPos(10, 70 + img1.getHeight());
	winPlots.resize(500, 200);

	// Another window to show the descriptors themselves:
	CDisplayWindow::Ptr winptr2D_descr1, winptr2D_descr2;
	CDisplayWindowPlots::Ptr winptrPlot_descr1, winptrPlot_descr2;

	if (fext.options.featsType == featSIFT) desc_to_compute = descSIFT;
	else if (fext.options.featsType == featSURF)
		desc_to_compute = descSURF;

	switch (desc_to_compute)
	{
		case descAny:  // Patch
		case descPolarImages:
		case descLogPolarImages:
		case descSpinImages:
		{
			winptr2D_descr1 = std::make_shared<CDisplayWindow>("Descriptor 1");
			winptr2D_descr1->setPos(550, 70 + img1.getHeight());
			winptr2D_descr1->resize(220, 200);

			winptr2D_descr2 = std::make_shared<CDisplayWindow>("Descriptor 2");
			winptr2D_descr2->setPos(760, 70 + img1.getHeight());
			winptr2D_descr2->resize(220, 200);
		}
		break;
		case descSIFT:
		case descSURF:
		{
			winptrPlot_descr1 =
				std::make_shared<CDisplayWindowPlots>("Descriptor 1");
			winptrPlot_descr1->setPos(550, 70 + img1.getHeight());
			winptrPlot_descr1->resize(220, 200);

			winptrPlot_descr2 =
				std::make_shared<CDisplayWindowPlots>("Descriptor 2");
			winptrPlot_descr2->setPos(760, 70 + img1.getHeight());
			winptrPlot_descr2->resize(220, 200);
		}
		break;
		default:
		{
			cerr << "Descriptor specified is not handled yet" << endl;
		}
		break;
	}

	CImage img1_show, img2_show, img2_show_base;

	img1_show.selectTextFont("6x13");
	img2_show.selectTextFont("6x13");
	img2_show_base.selectTextFont("6x13");

	// Show features distances:
	for (unsigned int i1 = 0; i1 < feats1.size() && winPlots.isOpen() &&
		 win1.isOpen() && win2.isOpen();
		 i1++)
	{
		// Compute distances:
		CVectorDouble distances(feats2.size());

		const auto& ft_i1 = feats1[i1];

		tictac.Tic();
		if (desc_to_compute != descAny)
		{
			// Ignore rotations
			// ft_i1.descriptors.polarImgsNoRotation = true;

			for (unsigned int i2 = 0; i2 < feats2.size(); i2++)
				distances[i2] = ft_i1.descriptorDistanceTo(feats2[i2]);
		}
		else
		{
			for (unsigned int i2 = 0; i2 < feats2.size(); i2++)
				distances[i2] = ft_i1.patchCorrelationTo(feats2[i2]);
		}
		cout << "All distances computed in " << 1000.0 * tictac.Tac() << " ms"
			 << endl;

		// Show Distances;
		winPlots.plot(distances, ".4k", "all_dists");

		std::size_t min_dist_idx = 0, max_dist_idx = 0;
		const double min_dist = distances.minCoeff(min_dist_idx);
		const double max_dist = distances.maxCoeff(max_dist_idx);

		const double dist_std = mrpt::math::stddev(distances);

		cout << "Min. distance=" << min_dist << " for img2 feat #"
			 << min_dist_idx << " .Distances sigma: " << dist_std << endl;

		winPlots.axis(-15, distances.size(), -0.15 * max_dist, max_dist * 1.15);
		winPlots.plot(
			std::vector<double>({1.0, double(min_dist_idx)}),
			std::vector<double>({1.0, min_dist}), ".8b", "best_dists");

		winPlots.setWindowTitle(
			format("Distances feat #%u -> all others ", i1));

		const auto& best_ft2 = feats2[min_dist_idx];

		// Display the current descriptor in its window and the best descriptor
		// from the other image:
		switch (desc_to_compute)
		{
			case descAny:  // Patch
			case descPolarImages:
			case descLogPolarImages:
			case descSpinImages:
			{
				CImage auxImg1, auxImg2;
				if (desc_to_compute == descAny)
				{
					auxImg1 = *ft_i1.patch;
					auxImg2 = *best_ft2.patch;
				}
				else if (desc_to_compute == descPolarImages)
				{
					auxImg1.setFromMatrix(*ft_i1.descriptors.PolarImg);
					auxImg2.setFromMatrix(*best_ft2.descriptors.PolarImg);
				}
				else if (desc_to_compute == descLogPolarImages)
				{
					auxImg1.setFromMatrix(*ft_i1.descriptors.LogPolarImg);
					auxImg2.setFromMatrix(*best_ft2.descriptors.LogPolarImg);
				}
				else if (desc_to_compute == descSpinImages)
				{
					{
						const size_t nR = ft_i1.descriptors.SpinImg_range_rows;
						const size_t nC = ft_i1.descriptors.SpinImg->size() /
							ft_i1.descriptors.SpinImg_range_rows;
						CMatrixFloat M1(nR, nC);
						for (size_t r = 0; r < nR; r++)
							for (size_t c = 0; c < nC; c++)
								M1(r, c) =
									(*ft_i1.descriptors.SpinImg)[c + r * nC];
						auxImg1.setFromMatrix(M1);
					}
					{
						const size_t nR =
							best_ft2.descriptors.SpinImg_range_rows;
						const size_t nC = best_ft2.descriptors.SpinImg->size() /
							best_ft2.descriptors.SpinImg_range_rows;
						CMatrixFloat M2(nR, nC);
						for (size_t r = 0; r < nR; r++)
							for (size_t c = 0; c < nC; c++)
								M2(r, c) =
									(*best_ft2.descriptors.SpinImg)[c + r * nC];
						auxImg2.setFromMatrix(M2);
					}
				}

				while (auxImg1.getWidth() < 100 && auxImg1.getHeight() < 100)
					auxImg1.scaleImage(
						auxImg1, auxImg1.getWidth() * 2,
						auxImg1.getHeight() * 2, IMG_INTERP_NN);
				while (auxImg2.getWidth() < 100 && auxImg2.getHeight() < 100)
					auxImg2.scaleImage(
						auxImg2, auxImg2.getWidth() * 2,
						auxImg2.getHeight() * 2, IMG_INTERP_NN);
				winptr2D_descr1->showImage(auxImg1);
				winptr2D_descr2->showImage(auxImg2);
			}
			break;
			case descSIFT:
			{
				vector<float> v1, v2;
				mrpt::containers::copy_container_typecasting(
					*ft_i1.descriptors.SIFT, v1);
				mrpt::containers::copy_container_typecasting(
					*best_ft2.descriptors.SIFT, v2);
				winptrPlot_descr1->plot(v1);
				winptrPlot_descr2->plot(v2);
				winptrPlot_descr1->axis_fit();
				winptrPlot_descr2->axis_fit();
			}
			break;
			case descSURF:
			{
				winptrPlot_descr1->plot(*ft_i1.descriptors.SURF);
				winptrPlot_descr2->plot(*best_ft2.descriptors.SURF);
				winptrPlot_descr1->axis_fit();
				winptrPlot_descr2->axis_fit();
			}
			break;
			default:
			{
				cerr << "Descriptor specified is not handled yet" << endl;
			}
			break;
		}

		// win2: Show only best matches:

		// CFeatureList  feats2_best;
		img2_show_base = img2.makeDeepCopy();

		CVectorDouble xs_best, ys_best;
		for (unsigned int i2 = 0; i2 < feats2.size(); i2++)
		{
			if (distances[i2] < min_dist + 0.3 * dist_std)
			{
				img2_show_base.drawMark(
					feats2[i2].keypoint.pt.x, feats2[i2].keypoint.pt.y,
					TColor::red(), '+', 7);

				img2_show_base.textOut(
					feats2[i2].keypoint.pt.x + 10,
					feats2[i2].keypoint.pt.y - 10,
					format("#%u, dist=%.02f", i2, distances[i2]),
					TColor::gray());

				xs_best.push_back(i2);
				ys_best.push_back(distances[i2]);
			}
			else
			{
				img2_show_base.drawMark(
					feats2[i2].keypoint.pt.x, feats2[i2].keypoint.pt.y,
					TColor::gray(), '+', 3);
			}
		}

		winPlots.plot(xs_best, ys_best, ".4b", "best_dists2");

		// Show new images in win1 / win2, but with a catchy animation to focus
		// on the features:
		// ------------------------------------------------------------------------------------------
		// win1: Show only the current feature:
		for (unsigned anim_loops = 36; anim_loops > 0; anim_loops -= 2)
		{
			img1_show = img1.makeDeepCopy();

			img1_show.drawMark(
				ft_i1.keypoint.pt.x, ft_i1.keypoint.pt.y, TColor::red(), '+',
				7);
			img1_show.drawCircle(
				ft_i1.keypoint.pt.x, ft_i1.keypoint.pt.y, 7 + anim_loops,
				TColor::blue());

			img2_show = img2_show_base.makeDeepCopy();
			for (unsigned int i2 = 0; i2 < feats2.size(); i2++)
			{
				if (distances[i2] < min_dist + 0.1 * dist_std)
				{
					img2_show.drawCircle(
						feats2[i2].keypoint.pt.x, feats2[i2].keypoint.pt.y,
						7 + anim_loops, TColor::blue());
				}
			}

			win1.showImage(img1_show);
			win2.showImage(img2_show);

			std::this_thread::sleep_for(10ms);
		}

		// Wait for the next iteration:
		cout << "Press any key on the distances window or on the console to "
				"continue (close any window to exit)..."
			 << endl;
		winPlots.waitForKey();
	}

	return false;
}

int main(int argc, char** argv)
{
	try
	{
		if (argc != 1 && argc != 3)
		{
			cerr << "Usage: " << endl;
			cerr << argv[0] << endl;
			cerr << argv[0] << " <image1> <image2>" << endl;
			return 1;
		}

		if (argc == 3)
		{
			file1 = string(argv[1]);
			file2 = string(argv[2]);
		}

		DemoFeatures();
		return 0;
	}
	catch (exception& e)
	{
		cerr << mrpt::exception_to_str(e);
		return 1;
	}
}
