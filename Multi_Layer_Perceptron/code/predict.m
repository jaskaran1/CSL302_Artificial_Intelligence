function p=predict(Theta1,Theta2,X)
%X doesn't contain the labels column in it.
%p are the predicted labels
m = size(X, 1);

% You need to return the following variables correctly 
h1 = sigmoid([ones(m, 1) X] * Theta1');%size is mX(#of nodes in hidden layer)
h2 = sigmoid([ones(m, 1) h1] * Theta2');%size is mXk
[~, p] = max(h2, [], 2);%the maximum value
%p is of the size mX1 where p(i) is the predicted label of the ith training
%example
end