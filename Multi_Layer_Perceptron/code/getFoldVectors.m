function [X_train, y_train, X_val, y_val] = getFoldVectors(X_sorted, y_sorted, ...
                                                categories, vecsPerCat, ...
                                                foldSizes, roundNumber)
%selects the vectors to use for training and validation for the
% specified round number.

X_train = [];
y_train = [];
X_val = [];
y_val = [];


% verify the vectors are properly sorted.
catStart = 1;
%Loop for verification
% For each category...
for (i = 1 : size(categories,1))
    
    % Compute the index of the last vector of this category.
    catEnd = catStart + vecsPerCat(i) - 1;

    % Verify that all of the vectors in the range catStart : catEnd have 
    % the expected category.
    if (any(y_sorted(catStart : catEnd) ~= categories(i)))
        disp('Input vectors are not properly sorted!');
        return;
    end
    
    % Set the starting index of the next category.
    catStart = catEnd + 1;
end


% Get the number of folds from the foldSizes matrix.
numFolds = size(foldSizes,2);

catStart = 1;

% For each category...
for (catIndex = 1 : size(categories,1))

    % Get the list of fold sizes for this category as a column vector.
    catFoldSizes = foldSizes(catIndex, :)';
    %catFoldSizes is a column vector of dimension #foldsX1 where each
    %element catFoldSizes(i,1) of the vector is the number of elements of the category in
    %fold i
    % Set the starting index of the first fold for this category.
    foldStart = catStart;
    
    % For each fold...
    for (foldIndex = 1 : numFolds)
        
        % Compute the index of the last vector in this fold.
        foldEnd = foldStart + catFoldSizes(foldIndex) - 1;
        
        % Select all of the vectors in this fold.
        foldVectors = X_sorted(foldStart : foldEnd, :);
        foldCats = y_sorted(foldStart : foldEnd, :);
        
        % If this fold is to be used for validation in this round...
        % The fold with the index same as round number is used as the
        % validation set for that round
        % and the rest of the folds are used in the training set.
        if (foldIndex == roundNumber)
            % Append the vectors to the validation set.
            X_val = [X_val; foldVectors];
            y_val = [y_val; foldCats];
        % Otherwise, use the fold for training.
        else
            % Append the vectors to the training set.
            X_train = [X_train; foldVectors];
            y_train = [y_train; foldCats];
        end
        
        % Update the starting index of the next fold.
        foldStart = foldEnd + 1;
    end
    
    % Set the starting index of the next category.
    catStart=catStart+vecsPerCat(catIndex);   
end

end