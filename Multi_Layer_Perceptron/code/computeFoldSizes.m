function [foldSizes] = computeFoldSizes(vecsPerCat, numFolds)

% Get the number of categories present.
numCats = size(vecsPerCat,1);

% For each category...
%Check if the numVecs>=numFolds
for i = 1 : numCats
    
    % Get the number of vectors for this category;
    numVecs = vecsPerCat(i, 1);
    
    % Verify that there are at least 'numFolds' samples.
    if (numVecs < numFolds)
        disp('ERROR! Each category must have at least "numFolds" samples.');
        return;
    end
end
    
% foldSizes will be a matrix holding the number of vectors to place in each fold
% for each category. The number of folds may not divide evenly into the number
% of vectors, so we need to distribute the remainder.
foldSizes = zeros(numCats, numFolds);
%Populate the foldSizes matrix rowwise where each row is the category and
%each element foldSizes(i,j) denotes the number of elements in the ith
%category and the jth fold
% For each category...
for (i = 1 : numCats)
    
    % Get the the number of vectors for this category.
    numVecs = vecsPerCat(i, 1);
            
    % For each of the ten folds...
    for (fold = 1 : numFolds)
        
        % Divide the remaining number of vectors by the remaining number of folds.
        foldSize = ceil(numVecs / (numFolds - fold + 1));
        
        % Store the fold size.
        foldSizes(i, fold) = foldSize;
        
        % Update the number of remaining vectors for this category.
        numVecs =numVecs-foldSize;
    end
end

% Verify the fold sizes sum up correctly.
if (any(sum(foldSizes, 2) ~= vecsPerCat))
    disp('ERROR! The sum of fold sizes did not equal the number of category vectors.');
    return;
end

end