/**
 * @file main.cpp
 *
 * @date 09.03.2025
 * @author mltya.usov04\@gmail.com
 * 
 * Copyright (c) 2025 Sireth
 */

#include <QApplication>
#include <QMainWindow>
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QChartView>
#include <QVBoxLayout>
#include <QTimer>
#include <CL/cl2.hpp>
#include <rxcpp/rx.hpp>
#include <vector>
#include <iostream>

const std::string kernelSource = R"(
__kernel void heat_eq(__global float* u, __global float* u_new, float alpha, float dx, float dt, int N) {
    int i = get_global_id(0);
    if (i > 0 && i < N - 1) {
        u_new[i] = u[i] + alpha * dt / (dx * dx) * (u[i-1] - 2 * u[i] + u[i+1]);
    }
}
)";

cl::Context createContext() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty()) throw std::runtime_error("No OpenCL platforms found.");
    cl::Platform platform = platforms[0];

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    if (devices.empty()) throw std::runtime_error("No GPU devices found.");
    cl::Device device = devices[0];

    return cl::Context(device);
}

cl::Program createProgram(const cl::Context& context, const std::string& source) {
    cl::Program program(context, source);
    program.build();
    return program;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Параметры задачи
    const int N = 100; // Количество точек
    const float L = 5.0f; // Длина стержня
    const float dx = L / (N - 1);
    const float alpha = 0.01f; // Коэффициент теплопроводности
    const float dt = 0.0001f; // Шаг по времени

    // Инициализация OpenCL
    cl::Context context = createContext();
    cl::Program program = createProgram(context, kernelSource);
    cl::CommandQueue queue(context);
    cl::Kernel kernel(program, "heat_eq");

    // Инициализация данных
    std::vector<float> u(N, 0.0f);
    u[N / 2] = 1.0f; // Начальное условие: тепло в центре
    std::vector<float> u_new(N, 0.0f);

    cl::Buffer u_buf(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, u.data());
    cl::Buffer u_new_buf(context, CL_MEM_READ_WRITE, sizeof(float) * N);

    // Инициализация Qt
    QMainWindow window;
    QChart chart;
    QLineSeries series;
    QValueAxis axisX, axisY;
    axisX.setRange(0, N);
    axisY.setRange(0, 1.0);
    chart.addSeries(&series);
    chart.setAxisX(&axisX, &series);
    chart.setAxisY(&axisY, &series);
    QChartView chartView(&chart);
    window.setCentralWidget(&chartView);
    window.resize(800, 600);
    window.show();

    // RxCpp для управления потоком данных
    auto scheduler = rxcpp::observe_on_event_loop();
    auto values = rxcpp::observable<>::interval(std::chrono::milliseconds(10), scheduler)
        .map([&](int) {
            // Выполнение OpenCL-кода
            kernel.setArg(0, u_buf);
            kernel.setArg(1, u_new_buf);
            kernel.setArg(2, alpha);
            kernel.setArg(3, dx);
            kernel.setArg(4, dt);
            kernel.setArg(5, N);
            queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(N), cl::NullRange);
            queue.enqueueReadBuffer(u_new_buf, CL_TRUE, 0, sizeof(float) * N, u.data());
            std::swap(u_buf, u_new_buf);
            return u;
        });

    // Обновление графика
    values.subscribe([&](const std::vector<float>& u) {
        series.clear();
        for (int i = 0; i < N; ++i) {
            series.append(i, u[i]);
        }
    });

    return app.exec();
}
