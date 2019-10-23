//
// Created by iclab on 10/23/19.
//


#include "parquet/api/reader.h"
#include "parquet/api/writer.h"
#include "parquet/exception.h"
#include <arrow/io/api.h>
#include "arrow/writer.h"
#include "arrow/status.h"
#include <iostream>
#include <string.h>
#include "tracer.h"
#include "arrow/api.h"
#include "arrow/array.h"
#include "arrow/type.h"

using namespace std;

void pseudoWriteInt64(char *outpath, int count) {
    Tracer tracer;
    tracer.startTime();
    arrow::Int64Builder i64builder;
    for (int i = 0; i < count; i++) {
        i64builder.Append(i);
    }
    std::shared_ptr<arrow::Array> i64array;
    i64builder.Finish(&i64array);
    std::shared_ptr<arrow::Schema> schema = arrow::schema({arrow::field("int", arrow::int64())});
    /*std::shared_ptr<arrow::Schema> schema = arrow::schema(
            {arrow::field("int", arrow::int64()), arrow::field("str", arrow::uint8())});*/
    std::shared_ptr<arrow::Table> table = arrow::Table::Make(schema, {i64array});

    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    arrow::io::FileOutputStream::Open(outpath, &outfile);
    parquet::arrow::WriteTable(*table, arrow::default_memory_pool(), outfile, 32768);
    cout << "Int64 time: " << tracer.getRunTime() << endl;
}

void pseudoWriteString(char *outpath, int count) {
    Tracer tracer;
    tracer.startTime();
    arrow::StringBuilder strbuilder;
    for (int i = 0; i < count; i++) {
        strbuilder.Append("Hello world, I am michael.");
    }
    std::shared_ptr<arrow::Array> strarray;
    strbuilder.Finish(&strarray);
    std::shared_ptr<arrow::Schema> schema = arrow::schema({arrow::field("str", arrow::int8())});
    std::shared_ptr<arrow::Table> table = arrow::Table::Make(schema, {strarray});

    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    arrow::io::FileOutputStream::Open(outpath, &outfile);
    parquet::arrow::WriteTable(*table, arrow::default_memory_pool(), outfile, 32768);
    cout << "String(26) time: " << tracer.getRunTime() << endl;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        cout << "Command output number";
        exit(0);
    }
    pseudoWriteInt64(argv[1], std::atoi(argv[2]));
    pseudoWriteString(argv[1], std::atoi(argv[2]));
}