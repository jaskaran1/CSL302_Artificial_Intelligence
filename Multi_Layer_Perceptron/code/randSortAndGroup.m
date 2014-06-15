function [X_sorted, y_sorted] = randSortAndGroup(X,y,categories)
%randomly sort the vectors in X, then group them by category.

%get the total number of input vectors.
totalVecs = size(X,1);

%get a random order of the indeces.
randOrder = randperm(totalVecs)';

%sort the vectors and categories with the random order.
randVecs = X(randOrder, :);
randCats = y(randOrder, :);

X_sorted = [];
y_sorted = [];

%re-group the vectors according to category.
for (i = 1 : size(categories,1))
    
    %get the next category value.
    cat = categories(i);
    
    %select all of the vectors for this category.
    catVecs = randVecs((randCats == cat), :);%Used logical indexing
    catCats = randCats((randCats == cat), :);
   
    %append the vectors for this category.
    X_sorted = [X_sorted; catVecs];
    y_sorted = [y_sorted; catCats];
end

end