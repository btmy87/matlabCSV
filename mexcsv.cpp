#include "mex.hpp"
#include "mexAdapter.hpp"

#include <cstddef>
#include <string>
#include <fstream>
#include <stdlib.h>

// use standard from_chars if available
// and not using fast_float
#ifdef HAVE_FROM_CHARS
#ifndef USE_FAST_FLOAT
#include <charconv>
using std::from_chars;
using std::from_chars_result;
#endif
#endif

#ifdef USE_FAST_FLOAT
// define HAVE_FROM_CHARS for use below
#ifndef HAVE_FROM_CHARS
#define HAVE_FROM_CHARS
#endif
#include "fast_float/fast_float.h"
using fast_float::from_chars;
using fast_float::from_chars_result;
#endif

class MexFunction : public matlab::mex::Function
{
  matlab::data::ArrayFactory factory;
  const std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();

public:
  void operator()(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs)
  {
    checkArguments(outputs, inputs);
    matlab::data::CharArray filename0 = inputs[0];
    std::string filename = filename0.toAscii();
    const size_t rowVarNames = inputs[1][0];
    const size_t rowsSkip = inputs[2][0];

    // open file
    std::ifstream inFile(filename);
    if (!inFile.is_open())
      mexPrintError("Could not open file " + filename);

    size_t nVars = readHeader(inFile, outputs, rowVarNames);
    skipRows(inFile, rowsSkip);

    char *buf = NULL;
    size_t nbuf;
    readData(inFile, &buf, nbuf);
    parseData(buf, nbuf, nVars, outputs);

    // close file
    inFile.close();
    delete[] buf;
  }

  size_t readHeader(std::ifstream &inFile, matlab::mex::ArgumentList &outputs, size_t rowVarNames)
  {
    std::string line;
    size_t irow = 0;
    while (irow < rowVarNames && std::getline(inFile, line))
    {
      // skip rows up to rowVarNames
      ++irow;
    }
    std::getline(inFile, line);
    // data from rowVarNames should be in 'line'
    std::istringstream lineStream(line);
    std::vector<matlab::data::CharArray> varNames;
    std::string tempName;
    while (std::getline(lineStream, tempName, ','))
    {
      varNames.push_back(factory.createCharArray(tempName));
    }

    // put into output cell array
    size_t nVars = varNames.size();
    outputs[1] = factory.createCellArray({1, nVars});
    for (size_t i = 0; i < nVars; ++i)
    {
      outputs[1][i] = varNames[i];
    }

    return nVars;
  };

  void skipRows(std::ifstream &inFile, size_t rowsSkip)
  {
    std::string line;
    size_t irow = 0;
    while (irow < rowsSkip && std::getline(inFile, line))
    {
      ++irow;
    }
  }

  void readData(std::ifstream &inFile, char **buf, size_t &nbuf)
  {
    // read rest of file into buffer
    size_t pos = inFile.tellg();
    inFile.seekg(0, std::ios::end);
    size_t fsize = ((size_t)inFile.tellg()) - pos;
    inFile.seekg(pos, std::ios::beg);

    nbuf = fsize + 1;
    *buf = new char[nbuf]{};

    inFile.read(*buf, fsize);
    (*buf)[fsize] = '\0'; // null terminate
  }

  void parseData(char *buf, size_t nbuf, size_t nVars, matlab::mex::ArgumentList &outputs)
  {

    std::vector<std::vector<double>> dataCols(nVars);
    const char *bufend = buf + nbuf;
    char *ibuf = buf;
    char *ibufnew = buf;
    while (ibuf < bufend && *ibuf != '\0')
    {
      for (size_t ivar = 0; ivar < nVars; ++ivar)
      {
        double temp;
        const char *ibuftemp = ibuf;
#ifdef HAVE_FROM_CHARS
        // use from_chars if available (C++17)
        from_chars_result res = from_chars(ibuftemp, bufend, temp);
        ibufnew = (char *)res.ptr + 1;
        dataCols[ivar].push_back(temp);
#else
        //  mingw 8.1 of g++ doesn't support from_chars for double
        dataCols[ivar].push_back(strtod(ibuf, &ibufnew));
#endif
        ibuf = ibufnew + 1; // skip comma
      }
    }

    outputs[0] = factory.createCellArray({1, nVars});
    for (int i = 0; i < nVars; ++i)
    {
      matlab::data::TypedArray<double> colArray = factory.createArray(
          {dataCols[i].size(), 1}, dataCols[i].begin(), dataCols[i].end(),
          matlab::data::InputLayout::COLUMN_MAJOR);
      outputs[0][i] = std::move(colArray);
    }
  }

  void checkArguments(matlab::mex::ArgumentList &outputs, matlab::mex::ArgumentList &inputs)
  {
    if (inputs.size() != 3)
      mexPrintError("Expected 3 inputs");
    if (outputs.size() > 2)
      mexPrintError("Expected at most 2 outputs");
  };

  void mexPrintError(std::string errMsg)
  {
    std::string errHeader("Error using mexcsv:\n");
    std::vector<matlab::data::Array> msgArray = {factory.createScalar(errHeader + errMsg)};
    matlabPtr->feval(u"error", 0, msgArray);
  };
};