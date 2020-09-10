from setuptools import setup, Extension


setup(
    name="tcptable",
    version="0.0.1",
    description="WinAPI GetExtendedTcpTable binding",
    author="André Sousa",
    author_email="andrematosdesousa@gmail.com",
    ext_modules=[
        Extension(
            "tcptable", 
            [
            "tcptable/windows/tcptablemodule.c",
            ],
            libraries=['Iphlpapi', 'Ws2_32']
        )
    ],
    license="MIT"
)
