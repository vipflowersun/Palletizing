# ============================================
# Install.cmake - 三方库DLL部署配置
# ============================================
# 由根 CMakeLists.txt 通过 include() 调用
# 依赖变量: PROJECT_ROOT_DIR, PROJECT_OUTPUT_DIR, QT_VERSION, OpenCV_DIR, PCL_DIR
# ============================================

# ============================================
# 辅助函数：为指定配置安装文件/目录到对应输出目录
# ============================================

# install_files_for_config(<config> <dest_subdir> <file1> [file2 ...])
#   将文件列表安装到 ${PROJECT_OUTPUT_DIR}/<dest_subdir>
function(install_files_for_config config dest_subdir)
    set(files ${ARGN})
    if(files)
        install(FILES ${files}
            DESTINATION ${PROJECT_OUTPUT_DIR}/${dest_subdir}
            CONFIGURATIONS ${config}
            OPTIONAL
        )
    endif()
endfunction()

# install_dirs_for_config(<config> <dest_subdir> <dir1> [dir2 ...])
#   将目录内容安装到 ${PROJECT_OUTPUT_DIR}/<dest_subdir>
function(install_dirs_for_config config dest_subdir)
    set(dirs ${ARGN})
    if(dirs)
        install(DIRECTORY ${dirs}
            DESTINATION ${PROJECT_OUTPUT_DIR}/${dest_subdir}
            CONFIGURATIONS ${config}
            OPTIONAL
        )
    endif()
endfunction()

# ============================================
# 收集三方库 DLL 文件列表
# ============================================

# --- VTK DLL ---
file(GLOB VTK_DLL ${PROJECT_ROOT_DIR}/thirdParty/VTK/bin/*.dll)
set(VTK_DLL_DEBUG "")
set(VTK_DLL_RELEASE "")
foreach(dll ${VTK_DLL})
    if(${dll} MATCHES "gd.dll")
        list(APPEND VTK_DLL_DEBUG ${dll})
    else()
        list(APPEND VTK_DLL_RELEASE ${dll})
    endif()
endforeach()

# --- PCL DLL ---official pcl
#file(GLOB PCL_DLL "${PCL_DIR}/../bin/*.dll")
#set(PCL_DLL_DEBUG "")
#set(PCL_DLL_RELEASE "")
#foreach(dll ${PCL_DLL})
#    if(${dll} MATCHES "d\\.dll$")
#        list(APPEND PCL_DLL_DEBUG ${dll})
#    else()
#        list(APPEND PCL_DLL_RELEASE ${dll})
#    endif()
#endforeach()

#file(GLOB PCL_VTK_DLL "${PCL_DIR}/../3rdParty/VTK/bin/*.dll")
#set(PCL_VTK_DLL_DEBUG "")
#set(PCL_VTK_DLL_RELEASE "")
#foreach(dll ${PCL_VTK_DLL})
#    if(${dll} MATCHES "gd.dll")
#        list(APPEND PCL_VTK_DLL_DEBUG ${dll})
#    else()
#        list(APPEND PCL_VTK_DLL_RELEASE ${dll})
#    endif()
#endforeach()

#MY PCL
file(GLOB PCL_DLL ${PROJECT_ROOT_DIR}/thirdParty/PCL1.15.0_MY/bin/*.dll)
set(PCL_DLL_DEBUG "")
set(PCL_DLL_RELEASE "")
foreach(dll ${PCL_DLL})
    list(APPEND PCL_DLL_DEBUG ${dll})
    list(APPEND PCL_DLL_RELEASE ${dll})
endforeach()

# --- CGAL DLL ---
file(GLOB CGAL_FILES "${PROJECT_ROOT_DIR}/thirdParty/CGAL-6.1.1/auxiliary/gmp/bin/*.dll")

# --- VzNL DLL ---
file(GLOB VZNL_FILES "${PROJECT_ROOT_DIR}/thirdParty/VzNLSDK/bin/*.dll")

# --- Model files ---
file(GLOB MODEL_PART_FILES "${PROJECT_ROOT_DIR}/models/*.part")

# --- Ceres DLL ---
file(GLOB CERES_DLL_DEBUG
    ${PROJECT_ROOT_DIR}/thirdParty/ceres/bin/ceres-debug.dll
    ${PROJECT_ROOT_DIR}/thirdParty/ceres/bin/glogd.dll
    ${PROJECT_ROOT_DIR}/thirdParty/ceres/bin/gflags.dll
    ${PROJECT_ROOT_DIR}/thirdParty/ceres/bin/liblapack.dll
    ${PROJECT_ROOT_DIR}/thirdParty/ceres/bin/libblas.dll
    ${PROJECT_ROOT_DIR}/thirdParty/ceres/bin/libquadmath-0.dll
    ${PROJECT_ROOT_DIR}/thirdParty/ceres/bin/libgcc_s_sjlj-1.dll
    ${PROJECT_ROOT_DIR}/thirdParty/ceres/bin/libgfortran-3.dll
)
file(GLOB CERES_DLL_RELEASE
    ${PROJECT_ROOT_DIR}/thirdParty/ceres/bin/ceres.dll
    ${PROJECT_ROOT_DIR}/thirdParty/ceres/bin/glog.dll
    ${PROJECT_ROOT_DIR}/thirdParty/ceres/bin/gflags.dll
    ${PROJECT_ROOT_DIR}/thirdParty/ceres/bin/liblapack.dll
    ${PROJECT_ROOT_DIR}/thirdParty/ceres/bin/libblas.dll
    ${PROJECT_ROOT_DIR}/thirdParty/ceres/bin/libquadmath-0.dll
    ${PROJECT_ROOT_DIR}/thirdParty/ceres/bin/libgcc_s_sjlj-1.dll
    ${PROJECT_ROOT_DIR}/thirdParty/ceres/bin/libgfortran-3.dll
)

# ============================================
# 通用安装函数：按配置部署所有三方库
# ============================================
# _install_thirdparty_for_config(<config> <out_subdir>
#     JMC_SUBDIR <debug|release>
#     SARQWKCORE <dll_name>  SARQWKWIDGETS <dll_name>  SARRIBBON <dll_name>
#     ELAWIDGETTOOLS <dll_name>
#     VTK_DLLS <list>  PCL_DLLS <list>  PCL_VTK_DLLS <list>  CERES_DLLS <list>
#     OPENCV_DLL <dll_path>
# )
function(_install_thirdparty_for_config config out_subdir)
    cmake_parse_arguments(ARG ""
        "JMC_SUBDIR;SARQWKCORE;SARQWKWIDGETS;SARRIBBON;ELAWIDGETTOOLS;OPENCV_DLL"
        "VTK_DLLS;PCL_DLLS;PCL_VTK_DLLS;CERES_DLLS"
        ${ARGN}
    )

    # JMCClient + JMCServer + OCC
    install(DIRECTORY
        ${PROJECT_ROOT_DIR}/thirdParty/JMCClient/bin/${ARG_JMC_SUBDIR}/
        ${PROJECT_ROOT_DIR}/thirdParty/JMCServer/bin/${ARG_JMC_SUBDIR}/
        ${PROJECT_ROOT_DIR}/thirdParty/occ/bin/x64/${ARG_JMC_SUBDIR}/
        DESTINATION ${PROJECT_OUTPUT_DIR}/${out_subdir}
        CONFIGURATIONS ${config}
        OPTIONAL
    )

    # Qt DLLs
    install(DIRECTORY
        ${PROJECT_ROOT_DIR}/thirdParty/${QT_VERSION}/bin/
        DESTINATION ${PROJECT_OUTPUT_DIR}/${out_subdir}
        CONFIGURATIONS ${config}
        FILES_MATCHING PATTERN "*.dll"
    )

    # Qt mkspecs
    install(DIRECTORY
        ${PROJECT_ROOT_DIR}/thirdParty/${QT_VERSION}/mkspecs/
        DESTINATION ${PROJECT_OUTPUT_DIR}/${out_subdir}/mkspecs
        CONFIGURATIONS ${config}
        OPTIONAL
    )

    # Qt plugins
    install(DIRECTORY
        ${PROJECT_ROOT_DIR}/thirdParty/${QT_VERSION}/plugins/
        DESTINATION ${PROJECT_OUTPUT_DIR}/${out_subdir}/plugins
        CONFIGURATIONS ${config}
        OPTIONAL
    )

    # Models
    #install(FILES ${PROJECT_ROOT_DIR}/models/default.scene
    #    DESTINATION ${PROJECT_OUTPUT_DIR}/${out_subdir}/models
    #    CONFIGURATIONS ${config}
    #    OPTIONAL
    #)
    #install_files_for_config(${config} "${out_subdir}/models" ${MODEL_PART_FILES})

    # SARibbon
    install_files_for_config(${config} ${out_subdir}
        ${PROJECT_ROOT_DIR}/thirdParty/SARibbon/bin/${ARG_SARQWKCORE}
        ${PROJECT_ROOT_DIR}/thirdParty/SARibbon/bin/${ARG_SARQWKWIDGETS}
        ${PROJECT_ROOT_DIR}/thirdParty/SARibbon/bin/${ARG_SARRIBBON}
    )

    # ElaWidgetTools
    install_files_for_config(${config} ${out_subdir}
        ${PROJECT_ROOT_DIR}/thirdParty/ElaWidgetTools/bin/${ARG_ELAWIDGETTOOLS}
    )

    # ini
    install(DIRECTORY ${PROJECT_ROOT_DIR}/ini/ini
        DESTINATION ${PROJECT_OUTPUT_DIR}/${out_subdir}
        CONFIGURATIONS ${config}
        OPTIONAL
    )
    install(FILES ${PROJECT_ROOT_DIR}/ini/MYWeldConfig.ini
        DESTINATION ${PROJECT_OUTPUT_DIR}/${out_subdir}
        CONFIGURATIONS ${config}
        OPTIONAL
    )

    # VzNL
    install_files_for_config(${config} ${out_subdir} ${VZNL_FILES})

    # VTK
    install_files_for_config(${config} ${out_subdir} ${ARG_VTK_DLLS})

    # PCL
    install_files_for_config(${config} ${out_subdir} ${ARG_PCL_DLLS})

    # PCL VTK
    install_files_for_config(${config} ${out_subdir} ${ARG_PCL_VTK_DLLS})

    # Ceres
    install_files_for_config(${config} ${out_subdir} ${ARG_CERES_DLLS})

    # CGAL
    install_files_for_config(${config} ${out_subdir} ${CGAL_FILES})

    # weldAlgo
    install_files_for_config(${config} ${out_subdir}
        ${PROJECT_ROOT_DIR}/thirdParty/weldAlgo/lib/weldAlgo.dll
    )

    # OpenCV
    install_files_for_config(${config} ${out_subdir} ${ARG_OPENCV_DLL})
endfunction()

# ============================================
# Debug 配置
# ============================================
_install_thirdparty_for_config(Debug debug
    JMC_SUBDIR          debug
    SARQWKCORE          QWKCored.dll
    SARQWKWIDGETS       QWKWidgetsd.dll
    SARRIBBON           SARibbonBard.dll
    ELAWIDGETTOOLS      ElaWidgetToolsd.dll
    VTK_DLLS            ${VTK_DLL_DEBUG}
    PCL_DLLS            ${PCL_DLL_DEBUG}
    PCL_VTK_DLLS        ${PCL_VTK_DLL_DEBUG}
    CERES_DLLS          ${CERES_DLL_DEBUG}
    OPENCV_DLL          ${OpenCV_DIR}/x64/vc16/bin/opencv_world490d.dll
)

# ============================================
# Release 配置
# ============================================
_install_thirdparty_for_config(Release release
    JMC_SUBDIR          release
    SARQWKCORE          QWKCore.dll
    SARQWKWIDGETS       QWKWidgets.dll
    SARRIBBON           SARibbonBar.dll
    ELAWIDGETTOOLS      ElaWidgetTools.dll
    VTK_DLLS            ${VTK_DLL_RELEASE}
    PCL_DLLS            ${PCL_DLL_RELEASE}
    PCL_VTK_DLLS        ${PCL_VTK_DLL_RELEASE}
    CERES_DLLS          ${CERES_DLL_RELEASE}
    OPENCV_DLL          ${OpenCV_DIR}/x64/vc16/bin/opencv_world490.dll
)

# ============================================
# RelWithDebInfo 配置 (使用 Release 版本的 DLL)
# ============================================
_install_thirdparty_for_config(RelWithDebInfo relwithdebinfo
    JMC_SUBDIR          release
    SARQWKCORE          QWKCore.dll
    SARQWKWIDGETS       QWKWidgets.dll
    SARRIBBON           SARibbonBar.dll
    ELAWIDGETTOOLS      ElaWidgetTools.dll
    VTK_DLLS            ${VTK_DLL_RELEASE}
    PCL_DLLS            ${PCL_DLL_RELEASE}
    PCL_VTK_DLLS        ${PCL_VTK_DLL_RELEASE}
    CERES_DLLS          ${CERES_DLL_RELEASE}
    OPENCV_DLL          ${OpenCV_DIR}/x64/vc16/bin/opencv_world490.dll
)

# ============================================
# 以 SHARED IMPORTED 方式引入的 module 子模块 DLL 部署
# ============================================
# 自动扫描 ${PROJECT_ROOT_DIR}/module/* 下的 CMakeLists.txt，
# 识别声明为 "SHARED IMPORTED" 的子模块，并按配置安装其 bin/<config>/*.dll。
# 期望布局：module/<name>/bin/{debug|release|relwithdebinfo}/*.dll
# ============================================
function(_install_imported_module_dlls config bin_subdir out_subdir)
    file(GLOB _module_cmakes ${PROJECT_ROOT_DIR}/module/*/CMakeLists.txt)
    foreach(_cmake ${_module_cmakes})
        file(READ ${_cmake} _content)
        # 跳过未以 SHARED IMPORTED 形式声明的子模块
        if(NOT _content MATCHES "add_library\\([^\\)]*SHARED[ \t\r\n]+IMPORTED")
            continue()
        endif()

        get_filename_component(_module_dir ${_cmake} DIRECTORY)

        # DLL
        set(_dll_dir "${_module_dir}/bin/${bin_subdir}")
        if(IS_DIRECTORY ${_dll_dir})
            file(GLOB _module_dlls "${_dll_dir}/*.dll")
            if(_module_dlls)
                install_files_for_config(${config} ${out_subdir} ${_module_dlls})
            endif()
        endif()

        # PDB
        set(_pdb_dir "${_module_dir}/lib/${bin_subdir}")
        if(IS_DIRECTORY ${_pdb_dir})
            file(GLOB _module_pdbs "${_pdb_dir}/*.pdb")
            if(_module_pdbs)
                install_files_for_config(${config} ${out_subdir} ${_module_pdbs})
            endif()
        endif()
    endforeach()
endfunction()

_install_imported_module_dlls(Debug          debug          debug)
_install_imported_module_dlls(Release        release        release)
_install_imported_module_dlls(RelWithDebInfo relwithdebinfo relwithdebinfo)
