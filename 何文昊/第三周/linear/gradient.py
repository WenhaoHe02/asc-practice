x_data = [1.0, 2.0, 3.0]
y_data = [2.0, 4.0, 6.0]
w = 1.0
def forward(x):
    return x * w


def cost(xs, ys):
    cost = 0
    for x, y in zip(xs, ys):
        y_pred = forward(x)
        cost += (y_pred - y)**2
    return cost / len(xs)


def gradient(xs, ys):
    grad = 0
    for x, y in zip(xs, ys):
        y_pred = forward(x)
        grad += 2 * x *(y_pred - y)
    return grad / len(xs)


print('predict(before training)',4,forward(4))
alpha = 0.01
for epoch in range(100000):
    cost_val = cost(x_data, y_data)
    grad_val = gradient(x_data, y_data)
    w -= alpha * grad_val
    print('Epch:', epoch, 'w=', w, 'loss=', cost_val)
    print('Prediect(after training)', 4, forward(4))

