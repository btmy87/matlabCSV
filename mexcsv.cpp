#include "mex.hpp"
#include "mexAdapter.hpp"

#include <cstddef>
#include <string>
#include <fstream>

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
    readData(inFile, nVars, outputs);

    // close file
    inFile.close();
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

  void readData(std::ifstream &inFile, size_t nVars, matlab::mex::ArgumentList &outputs)
  {
    std::vector<std::vector<double>> dataCols(nVars);
    std::string line;
    while (std::getline(inFile, line))
    {
      std::istringstream lineStream(line);
      std::string tempVal;
      size_t icol = 0;
      while (icol < nVars && std::getline(lineStream, tempVal, ','))
      {
        try
        {
          double val = std::stod(tempVal);
          dataCols[icol].push_back(val);
        }
        catch (const std::invalid_argument &)
        {
          // non-numeric data, put in NaN
          dataCols[icol].push_back(std::numeric_limits<double>::quiet_NaN());
        }
        ++icol;
      }
      // if fewer columns than nVars, fill in remaining with NaN
      while (icol < nVars)
      {
        dataCols[icol].push_back(std::numeric_limits<double>::quiet_NaN());
        ++icol;
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