#include "scan.h"

bool sortX(cv::Rect i, cv::Rect j) {
	return i.x < j.x;
}

cv::Mat convert_to_bw(cv::Mat src) {
	cv::Mat src_gray = cv::Mat(src.size(), CV_8U);
	cv::cvtColor(src, src_gray, CV_BGR2GRAY);

	cv::Mat src_bw = cv::Mat(src_gray.size(), src_gray.type());
	cv::threshold(src_gray, src_bw, 200, 255, cv::THRESH_BINARY);
	cv::bitwise_not(src_bw, src_bw);

	return src_bw;
};

std::vector<cv::Rect> locateSquares(cv::Mat src, ScanConfig config) {
	cv::Mat src_bw = convert_to_bw(src);

	std::vector<std::vector<cv::Point> > contours;
	cv::Mat contour_output = src_bw.clone();
	cv::findContours(contour_output, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	std::vector<cv::Rect> bounding_boxes;

	for (size_t i = 0; i < contours.size(); ++i) {
		cv::Rect bb = cv::boundingRect(contours[i]);

		if (bb.width < src_bw.cols * config.square_width * SQUARE_TOLERANCE_MIN &&
			bb.height < src_bw.rows * config.square_height * SQUARE_TOLERANCE_MIN &&
			bb.width > src_bw.cols * config.square_width * SQUARE_TOLERANCE_MAX &&
			bb.height > src_bw.rows * config.square_height * SQUARE_TOLERANCE_MAX) {
			bounding_boxes.push_back(bb);
		}
	}

	std::sort(bounding_boxes.begin(), bounding_boxes.end(), sortX);

	std::vector<cv::Rect> squares;
	squares.resize(4);

	if (bounding_boxes[0].y < bounding_boxes[1].y) {
		squares[0] = bounding_boxes[0];
		squares[3] = bounding_boxes[1];
	} else {
		squares[0] = bounding_boxes[1];
		squares[3] = bounding_boxes[0];
	}

	if (bounding_boxes[bounding_boxes.size() - 1].y < bounding_boxes[bounding_boxes.size() - 2].y) {
		squares[1] = bounding_boxes[bounding_boxes.size() - 1];
		squares[2] = bounding_boxes[bounding_boxes.size() - 2];
	} else {
		squares[1] = bounding_boxes[bounding_boxes.size() - 2];
		squares[2] = bounding_boxes[bounding_boxes.size() - 1];
	}

	return squares;
}

std::vector<double> readCircles(cv::Mat src, ScanConfig config, std::vector<double> circles) {
	std::vector<cv::Rect> squares = locateSquares(src, config);

	cv::Mat src_bw = convert_to_bw(src);

	cv::Point tl = cv::Point(squares[0].x + squares[0].width, squares[0].y + squares[0].height);
	cv::Point br = cv::Point(squares[2].x, squares[2].y);

	double width = abs(tl.x - br.x);
	double height = abs(tl.y - br.y);

	std::vector<double> output;

	for (size_t i = 0; i < circles.size(); i += 2) {
		double x = circles[i];
		double y = circles[i + 1];

		cv::Point2f bubble_tl = cv::Point2f(tl.x + (width * x),
											tl.y + (height * y));
		cv::Point2f bubble_br = cv::Point2f((width * config.circle_width) + bubble_tl.x,
											(height * config.circle_height) + bubble_tl.y);
		cv::Rect bubble = cv::Rect(bubble_tl, bubble_br);
		cv::Mat bubble_mat = src_bw(bubble);

		double darkness = (double) cv::countNonZero(bubble_mat) / (bubble_mat.cols * bubble_mat.rows);

		if (darkness > 0.5) {
			cv::rectangle(src, bubble, CV_RGB(0, 255, 0), 2);
		} else {
			cv::rectangle(src, bubble, CV_RGB(255, 0, 0), 2);
		}

		output.push_back(darkness);
	}

	cv::imwrite("./output.jpeg", src);

	return output;
}

cv::Mat autoRotate(cv::Mat src, ScanConfig config) {
	std::vector<cv::Rect> squares = locateSquares(src, config);

	std::vector<cv::Point2f> quad_pts;
	std::vector<cv::Point2f> square_pts;

	cv::Point tl = cv::Point(squares[0].x, squares[0].y);
	cv::Point tr = cv::Point(squares[1].x + squares[1].width, squares[1].y);
	cv::Point br = cv::Point(squares[2].x + squares[2].width, squares[2].y + squares[2].height);
	cv::Point bl = cv::Point(squares[3].x, squares[3].y + squares[3].height);

	quad_pts.push_back(tl);
	quad_pts.push_back(tr);
	quad_pts.push_back(br);
	quad_pts.push_back(bl);

	double ratio = 8.5 / 11;

	double width = src.rows * ratio;
	double height = src.rows;

	square_pts.push_back(cv::Point(width * 0.025, height * 0.025));
	square_pts.push_back(cv::Point(width * 0.975, height * 0.025));
	square_pts.push_back(cv::Point(width * 0.975, height * 0.975));
	square_pts.push_back(cv::Point(width * 0.025, height * 0.975));

	cv::Mat transmtx = cv::getPerspectiveTransform(quad_pts, square_pts);
	cv::Mat transformed = cv::Mat(width, height, CV_8UC3);
	cv::warpPerspective(src, transformed, transmtx, src.size(),	CV_INTER_LINEAR, cv::BORDER_CONSTANT, CV_RGB(255, 255, 255));

	return transformed;
}

std::vector<double> scan(std::string imagePath, ScanConfig config, std::vector<double> bubbles) {
	cv::Mat src = cv::imread(std::string(imagePath));

	cv::Mat rotated_src;
	src.copyTo(rotated_src);

	for (size_t i = 0; i < ROTATE_CYCLES; ++i) {
		rotated_src = autoRotate(rotated_src, config);
	}

	return readCircles(rotated_src, config, bubbles);
};