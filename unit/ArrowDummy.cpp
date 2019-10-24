//
// Created by iclab on 10/16/19.
//

#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
#include <parquet/exception.h>
#include "arrow/api.h"
#include "arrow/io/api.h"
#include "arrow/writer.h"
#include "arrow/status.h"
#include "gtest/gtest.h"
#include <iostream>
#include <string.h>

#include "arrow/api.h"
#include "arrow/array.h"
#include "arrow/type.h"

using namespace std;

TEST(ArrowTest, WriterTest) {
    arrow::Int64Builder i64builder;
    i64builder.AppendValues({1, 2, 3, 4, 5, 6});
    std::shared_ptr<arrow::Array> i64array;
    i64builder.Finish(&i64array);

    arrow::StringBuilder strbuilder;
    strbuilder.AppendValues({"first", "second", "third", "forth", "fifth", "six"});
    std::shared_ptr<arrow::Array> strarray;
    strbuilder.Finish(&strarray);
    std::shared_ptr<arrow::Schema> schema = arrow::schema(
            {arrow::field("int", arrow::int64()), arrow::field("str", arrow::uint8())});
    std::shared_ptr<arrow::Table> table = arrow::Table::Make(schema, {i64array, strarray});

    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    arrow::io::FileOutputStream::Open("../res/parquet/dummy.par", &outfile);
    parquet::arrow::WriteTable(*table, arrow::default_memory_pool(), outfile, 4096);
    ASSERT_EQ(table->num_rows(), 6);
}

TEST(ArrowTest, SchemaTest) {
    std::unique_ptr<parquet::ParquetFileReader> reader = parquet::ParquetFileReader::OpenFile(
            "../res/parquet/dummy.par");
    PrintSchema(reader->metadata()->schema()->schema_root().get(), std::cout);
}

TEST(ArrowTest, ReaderTest) {
    std::shared_ptr<arrow::io::ReadableFile> infile;
    arrow::io::ReadableFile::Open("../res/parquet/dummy.par", arrow::default_memory_pool(), &infile);

    std::unique_ptr<parquet::arrow::FileReader> freader;
    parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &freader);
    std::shared_ptr<arrow::Table> table;
    freader->ReadTable(&table);
    std::cout << "Loaded " << table->num_rows() << " rows in " << table->num_columns() << " columns." << std::endl;
}

TEST(ArrowTest, FilterTest) {

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}