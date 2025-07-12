function out = csv(filename, rowVarNames, rowsSkip)
% csv read csv and return matlab table
%
% INPUTS:
%   filename: 1xN char array, path to csv file
%   rowVarNames: row number (0-based) containing variable names, default=0
%   rowsSkip: number of rows to skip after variable names, default=0

arguments
  filename (1, 1) string {mustBeFile}
  rowVarNames (1, 1) {mustBeNonnegative} = 0
  rowsSkip (1, 1) {mustBeNonnegative} = 0
end

[vars, varnames] = mexcsv(char(filename), rowVarNames, rowsSkip);
varnames = strtrim(varnames);
out = table.init(vars,length(vars{1}),{},length(vars),varnames);
end