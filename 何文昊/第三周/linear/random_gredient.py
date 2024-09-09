x_data = [1.0, 2.0, 3.0]
y_data = [2.0, 4.0, 6.0]
w = 1.0
def forward(x):
    return x * w


def loss(x, y):
    cost = (forward(x) - y) ** 2
    return cost


def gradient(x, y):
    grad = 2 * x *(forward(x) - y)
    return grad


print('predict(before training)',4,forward(4))
alpha = 0.01
for epoch in range(100):
    for x, y in zip(x_data, y_data):
        grad_val = gradient(x, y)
        loss_val = loss(x, y)
        w -= alpha * grad_val
    print('Epch:', epoch, 'w=', w, 'loss=', loss_val)
    print('Prediect(after training)', 4, forward(4))