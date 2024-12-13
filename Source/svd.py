import numpy as np
import numpy.linalg as la
import matplotlib.pyplot as plt
import math
import time

# Функция для генерации вектора Лежандра
def lezhandr(epsilon, N):
    lezhandr_p = [1, epsilon]
    for i in range(2, N):
        lezhandr_p.append((2 * i + 1) / (i + 1) * epsilon * lezhandr_p[i - 1] - i / (i + 1) * lezhandr_p[i - 2])
    return lezhandr_p

def householder_qr_decomp(A):
    """
    Оптимизированное QR-разложение матрицы A с использованием метода Хаусхолдера.
    """
    m, n = A.shape
    Q = np.eye(m)
    R = A.copy()

    for k in range(min(m, n)):
        # Вычисляем вектор отражения
        x = R[k:, k]
        u = householder_reflection_vector(x)
        
        # Обновляем R только в подматрице
        R[k:, k:] -= 2 * np.outer(u, u @ R[k:, k:])
        
        # Обновляем Q только в подматрице
        Q[:, k:] -= 2 * np.outer(Q[:, k:] @ u, u)

    return Q, R

def householder_reflection_vector(x):
    """
    Вычисляет вектор отражения Хаусхолдера для заданного вектора x.
    """
    e = np.zeros_like(x)
    e[0] = np.linalg.norm(x)
    u_tilde = x + np.sign(x[0]) * e
    norm_u_tilde = np.linalg.norm(u_tilde)
    if norm_u_tilde == 0:
        return u_tilde
    u = u_tilde / norm_u_tilde
    return u

def svd_without_eigen(A, tol=1e-9, max_iter=100):
    """Реализация SVD без вычисления собственных значений."""
    m, n = A.shape
    U = np.eye(m)
    V = np.eye(n)
    A_copy = A.copy()
    
    for _ in range(max_iter):
        # Шаг QR-разложения с использованием метода Хаусхолдера
        Q, R = householder_qr_decomp(A_copy)
        U = U @ Q
        A_copy = R
        
        Q, R = householder_qr_decomp(A_copy.T)
        V = V @ Q
        A_copy = R.T

        # Проверка на сходимость
        diag_A = np.zeros_like(A_copy)
        np.fill_diagonal(diag_A, np.diagonal(A_copy))
        off_diag_norm = np.linalg.norm(A_copy - diag_A)
        if off_diag_norm < tol:
            break

    # Диагональные элементы A_copy -> сингулярные значения
    Sigma = np.zeros((m, n))
    for i in range(min(m, n)):
        Sigma[i, i] = np.abs(A_copy[i, i])

    return U, Sigma, V.T

epsilon = []
sigma = []

file = './Data/mmcm1/data_4.txt'
with open(file, 'r', encoding='utf-8-sig') as f:
    for line in f:
        parts = line.split()
        epsilon.append(float(parts[0]))
        sigma.append(float(parts[1]))

M = len(epsilon)
N = 10

matrix_A = np.array([lezhandr(e, N) for e in epsilon])
b = np.array(sigma)

print(f"Размеры матрицы: {matrix_A.shape}")
print("Количество элементов в epsilon:", len(epsilon))

# # Решение системы
# start_time = time.time()
# x = solve_normal_equations(matrix_A, b)
# execution_time = time.time() - start_time
# print(f"Время выполнения программы: {execution_time} секунд")
# print("Решение x:", x)

# Использование SVD для проверки
start_time = time.time()
U, S, VT = svd_without_eigen(matrix_A)
x_svd = VT.T @ np.linalg.solve(np.diag(S) @ U.T, b)
execution_time = time.time() - start_time
print(f"Решение с использованием SVD: {x_svd}")
print(f"Время выполнения SVD: {execution_time} секунд")

# Графики
res_x = [lezhandr(epsilon[i], N) @ x.T for i in range(M)]
plt.plot(epsilon, sigma, color='blue', linewidth=1)
plt.plot(epsilon, res_x, color='red', linewidth=1)
plt.show()
