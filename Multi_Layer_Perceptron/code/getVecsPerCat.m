function [vecsPerCat] = getVecsPerCat(y, categories)
%get the number of vectors in X belonging to each category.
numCats = length(categories);
% 'vecsPerCat' will store the number of input vectors belonging to each category.
vecsPerCat = zeros(numCats, 1);

%for each category...
for (i = 1 : numCats)
    
    %get the ith category; store the category value in column 1.
    cat = categories(i);
    
    %count the number of input vectors with that category.
    vecsPerCat(i, 1) = sum(y == cat);    
end

end