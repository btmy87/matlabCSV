#include "mex.hpp"

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

    // read header
    std::string line;
    size_t irow = 0;
    while (irow < rowVarNames && std::getline(inFile, line))
    {
      // skip rows up to rowVarNames
      ++irow;
    }
    // data from rowVarNames should be in 'line'
    std::istringstream lineStream(line);
    std::vector<matlab::data::CharArray> varNames;
    std::string tempName;
    while (std::getline(lineStream, tempName, ','))
    {
      varNames.push_back(factory.createCharArray(tempName));
    }

    // put into output cell array
    outputs[1] = factory.createCellArray({1, varNames.size()});
    for (size_t i = 0; i < varNames.size(); ++i)
    {
      outputs[1][i] = varNames[i];
    }

    // read data
    // just a blank for now
    outputs[0] = factory.createCellArray({1, 1});

    // close file
    inFile.close();
  }

  void checkArguments(matlab::mex::ArgumentList &outputs, matlab::mex::ArgumentList &inputs)
  {
    if (inputs.size() != 0)
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