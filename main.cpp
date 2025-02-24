﻿#include <CLI/CLI.hpp>
#include <base/stream/IFileStream.h>
#include <base/task/CancellationTokenSource.h>
#include <iostream>
#include <tsduck/TSDumper.h>
#include <tsduck/io/TSPacketStreamReader.h>

using namespace video;
using namespace std;

int main(int argc, char **argv)
{
	string input_file_path;
	string output_file_path;
	CLI::App app{"输出 ts 文件的包、表格的信息到指定的文本文件中。"};

	try
	{
		// 定义输入文件选项
		app.add_option(
			   "-i,--input",
			   input_file_path,
			   "输入的 ts 文件")
			->required()
			->check(CLI::ExistingFile); // 确保输入文件存在

		// 定义输出文件选项
		app.add_option(
			   "-o,--output",
			   output_file_path,
			   "输出的文本文件")
			->required();

		app.parse(argc, argv);

		// 执行到这里就是解析命令行成功，并且命令行参数不是 -h,--help
		cout << "正在解析 ts" << endl;

		shared_ptr<base::IFileStream> input_file_stream = base::di::filesystem::OpenReadOnly(input_file_path);
		TSPacketStreamReader reader{input_file_stream};
		shared_ptr<TSDumper> ts_dumper{new TSDumper{output_file_path}};
		base::CancellationTokenSource cancel_pump_source;
		reader.PumpTo(ts_dumper, cancel_pump_source.Token());
		ts_dumper->DisplayStatisticalResults();
	}
	catch (const CLI::ParseError &e)
	{
		return app.exit(e);
	}
	catch (std::exception &e)
	{
		cerr << e.what() << endl;
	}

	return 0;
}
