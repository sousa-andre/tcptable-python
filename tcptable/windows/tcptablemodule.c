#include <Python.h>
#include <winsock.h>
#include <iphlpapi.h>


PyObject* GetCTcpConnections() {
    MIB_TCPTABLE_OWNER_PID* pTcpTable;
    DWORD dwSize = 0;

    IN_ADDR LocalIpAddr, RemoteIpAddr;
    int i;

    PyObject* pyDictObject;
    PyObject* pyTupleObject;

    pTcpTable = (MIB_TCPTABLE_OWNER_PID*) malloc(sizeof (MIB_TCPTABLE_OWNER_PID));
    if (pTcpTable == NULL) {
        return Py_BuildValue("");
    }

    dwSize = sizeof(MIB_TCPTABLE);
    if (GetExtendedTcpTable(pTcpTable, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) == ERROR_INSUFFICIENT_BUFFER) {
        free(pTcpTable);
        pTcpTable = (MIB_TCPTABLE_OWNER_PID*) malloc(dwSize);
        if (pTcpTable == NULL) {
            return Py_BuildValue("");
        }

        if (GetExtendedTcpTable(pTcpTable, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) == NO_ERROR) {
            pyTupleObject = PyTuple_New((Py_ssize_t) pTcpTable->dwNumEntries);

            for (i = 0 ; i < (int) pTcpTable->dwNumEntries ; i++) {
                LocalIpAddr.S_un.S_addr = (u_long) pTcpTable->table[i].dwLocalAddr;
                RemoteIpAddr.S_un.S_addr = (u_long) pTcpTable->table[i].dwRemoteAddr;

                pyDictObject = PyDict_New();
                PyDict_SetItem(
                    pyDictObject, 
                    Py_BuildValue("s", "local_address"), 
                    Py_BuildValue("s", inet_ntoa(LocalIpAddr))
                );
                PyDict_SetItem(
                    pyDictObject,
                    Py_BuildValue("s", "local_port"),
                    Py_BuildValue("i", pTcpTable->table[i].dwLocalPort)
                );

                PyDict_SetItem(
                    pyDictObject,
                    Py_BuildValue("s", "remote_address"),
                    Py_BuildValue("s", inet_ntoa(RemoteIpAddr))
                );
                PyDict_SetItem(
                    pyDictObject,
                    Py_BuildValue("s", "remote_port"),
                    Py_BuildValue("i", pTcpTable->table[i].dwRemotePort)
                );

                PyDict_SetItem(
                    pyDictObject,
                    Py_BuildValue("s", "owning_pid"),
                    Py_BuildValue("i", pTcpTable->table[i].dwOwningPid)
                );

                PyTuple_SetItem(pyTupleObject, (Py_ssize_t) i, pyDictObject);
            }
            return pyTupleObject;

        } else {
            return Py_BuildValue("");
        }
    } else {
        return Py_BuildValue("");
    }
}


static PyMethodDef TCPTableMethods[] = {
    {"get_tcp_table", GetCTcpConnections, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}
};


static PyModuleDef tcptablemodule = {
    PyModuleDef_HEAD_INIT,
    "tcptable",
    NULL,
    -1,
    TCPTableMethods
};

PyMODINIT_FUNC PyInit_tcptable() {
    return PyModule_Create(&tcptablemodule);
}