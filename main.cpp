main C++
#include <iostream>
#include <string>
#include <Python.h>

using namespace std;

// Hàm chuyển đổi chuỗi C++ thành đối số Python
PyObject* convert_string_to_python(const string& input_string) {
    // Tạo một đối tượng tuple để lưu trữ đối số
    PyObject* pArgs = PyTuple_New(1);
    if (!pArgs) {
        // Nếu không thể tạo được đối tượng tuple, trả về nullptr
        return nullptr;
    }

    // Chuyển đổi chuỗi C++ thành đối tượng Unicode Python
    PyObject* pStrArg = PyUnicode_FromString(input_string.c_str());
    if (!pStrArg) {
        // Nếu không thể chuyển đổi chuỗi, giải phóng bộ nhớ và trả về nullptr
        Py_DECREF(pArgs);
        return nullptr;
    }

    // Đặt đối tượng Unicode vào tuple
    PyTuple_SetItem(pArgs, 0, pStrArg);
    return pArgs;
}

// Hàm chuyển đổi kết quả từ đối tượng Python thành chuỗi C++
string convert_python_to_string(PyObject* pValue) {
    // Kiểm tra xem đối tượng có phải là chuỗi Unicode không
    if (!PyUnicode_Check(pValue)) {
        // Nếu không phải, trả về thông báo lỗi
        return "Kết quả không phải là một chuỗi Unicode.";
    }

    // Chuyển đổi đối tượng Unicode thành chuỗi C++
    const char* c_result = PyUnicode_AsUTF8(pValue);
    if (!c_result) {
        // Nếu không thể chuyển đổi, trả về thông báo lỗi
        return "Không thể chuyển đổi kết quả từ Python sang C++.";
    }

    // Trả về chuỗi C++
    return string(c_result);
}

// Hàm giải phóng bộ nhớ cho các đối tượng Python
void release_python_objects(PyObject* pModule, PyObject* pFunc, PyObject* pArgs, PyObject* pValue) {
    // Giải phóng từng đối tượng Python một cách an toàn
    Py_XDECREF(pModule);
    Py_XDECREF(pFunc);
    Py_XDECREF(pArgs);
    Py_XDECREF(pValue);
}

// Hàm bắt mạch sử dụng Python
string diagnose_disease(const string& input_data) {
    // Khởi tạo các biến Python cần thiết
    PyObject *pModule = nullptr, *pFunc = nullptr, *pArgs = nullptr, *pValue = nullptr;

    try {
        // Khởi tạo môi trường Python
        Py_Initialize();

        // Kiểm tra xem môi trường Python đã khởi tạo thành công chưa
        if (!Py_IsInitialized()) {
            throw runtime_error("Không thể khởi tạo môi trường Python.");
        }

        // Import module Python chứa hàm predict
        pModule = PyImport_ImportModule("diagnose");
        if (!pModule) {
            throw runtime_error("Không thể import module Python.");
        }

        // Lấy hàm predict từ module
        pFunc = PyObject_GetAttrString(pModule, "predict");
        if (!pFunc || !PyCallable_Check(pFunc)) {
            if (PyErr_Occurred()) PyErr_Print();
            throw runtime_error("Không tìm thấy hoặc không thể gọi hàm predict.");
        }

        // Chuyển đổi dữ liệu vào định dạng Python
        pArgs = convert_string_to_python(input_data);
        if (!pArgs) {
            throw runtime_error("Không thể tạo đối số cho hàm predict.");
        }

        // Gọi hàm predict từ Python và nhận kết quả
        pValue = PyObject_CallObject(pFunc, pArgs);
        if (!pValue) {
            PyErr_Print();
            throw runtime_error("Gọi hàm predict không thành công.");
        }

        // Chuyển đổi kết quả từ Python sang C++
        string diagnosis_result = convert_python_to_string(pValue);

        // Giải phóng bộ nhớ
        release_python_objects(pModule, pFunc, pArgs, pValue);

        // Đóng môi trường Python
        Py_Finalize();

        return diagnosis_result;
    } catch (const exception& e) {
        // Xử lý lỗi và giải phóng bộ nhớ
        cerr << "Lỗi: " << e.what() << endl;
        release_python_objects(pModule, pFunc, pArgs, pValue);
        Py_Finalize();
        return "Lỗi trong quá trình thực thi Python";
    }
}

// Hàm main
int main() {
    try {
        // Nhập dữ liệu từ người dùng
        cout << "Nhập dữ liệu: ";
        string input_data;
        getline(cin, input_data);

        // Bắt mạch và chuẩn đoán bệnh
        string diagnosis_result = diagnose_disease(input_data);

        // In kết quả
        cout << "Kết quả chuẩn đoán bệnh: " << diagnosis_result << endl;

        return 0;
    } catch (const exception& e) {
        // Xử lý lỗi nếu có
        cerr << "Lỗi: " << e.what() << endl;
        return 1;
    } catch (...) {
        // Xử lý lỗi không xác định
        cerr << "Lỗi không xác định trong chương trình chính." << endl;
        return 1;
    }
}

main python
# File này đặt tên là diagnose.py

# Hàm dự đoán bệnh sử dụng mô hình AI
def predict(input_data):
    try:
        # Xử lý dữ liệu và thực hiện dự đoán ở đây
        # Đây chỉ là một hàm giả định
        if "symptom" in input_data:
            return "Có thể bạn đang mắc bệnh XYZ."
        else:
            return "Không có dữ liệu đủ để chuẩn đoán."
    except Exception as e:
        raise RuntimeError("Lỗi trong quá trình dự đoán: " + str(e))

