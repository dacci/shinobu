// Copyright (c) 2016 dacci.org

#ifndef SHINOBU_MODULE_PERFMON_PERFORMANCE_MONITOR_PAGE_H_
#define SHINOBU_MODULE_PERFMON_PERFORMANCE_MONITOR_PAGE_H_

#include <atlbase.h>
#include <atlwin.h>

#include <atlapp.h>
#include <atlcrack.h>
#include <atlctrls.h>
#include <atlddx.h>
#include <atldlgs.h>

#include "res/resource.h"

class Application;

class PerformanceMonitorPage : public CPropertyPageImpl<PerformanceMonitorPage>,
                               public CWinDataExchange<PerformanceMonitorPage> {
 public:
  static const UINT IDD = IDD_PROPPAGE_PERFORMANCE_MONITOR;

  explicit PerformanceMonitorPage(Application* application);

  BEGIN_MSG_MAP(PerformanceMonitorPage)
    MSG_WM_INITDIALOG(OnInitDialog)

    CHAIN_MSG_MAP(CPropertyPageImpl)
  END_MSG_MAP()

  BEGIN_DDX_MAP(PerformanceMonitorPage)
    DDX_CONTROL_HANDLE(IDC_MONITOR_CPU, monitor_cpu_)
    DDX_INT(IDC_CPU_USAGE, cpu_usage_)
    DDX_CONTROL_HANDLE(IDC_CPU_USAGE_SPIN, cpu_usage_spin_)
    DDX_INT(IDC_CPU_TIME, cpu_time_)
    DDX_CONTROL_HANDLE(IDC_CPU_TIME_SPIN, cpu_time_spin_)
    DDX_CONTROL_HANDLE(IDC_CPU_TIME_UNIT, cpu_time_unit_)
    DDX_CONTROL_HANDLE(IDC_MONITOR_DISK, monitor_disk_)
    DDX_INT(IDC_DISK_USAGE, disk_usage_)
    DDX_CONTROL_HANDLE(IDC_DISK_USAGE_SPIN, disk_usage_spin_)
    DDX_INT(IDC_DISK_TIME, disk_time_)
    DDX_CONTROL_HANDLE(IDC_DISK_TIME_SPIN, disk_time_spin_)
    DDX_CONTROL_HANDLE(IDC_DISK_TIME_UNIT, disk_time_unit_)
    DDX_CONTROL_HANDLE(IDC_MONITOR_TRAFFIC, monitor_traffic_)
    DDX_INT(IDC_TRAFFIC, traffic_)
    DDX_CONTROL_HANDLE(IDC_TRAFFIC_SPIN, traffic_spin_)
    DDX_CONTROL_HANDLE(IDC_TRAFFIC_UNIT, traffic_unit_)
    DDX_INT(IDC_TRAFFIC_TIME, traffic_time_)
    DDX_CONTROL_HANDLE(IDC_TRAFFIC_TIME_SPIN, traffic_time_spin_)
    DDX_CONTROL_HANDLE(IDC_TRAFFIC_TIME_UNIT, traffic_time_unit_)
  END_DDX_MAP()

  void OnFinalMessage(HWND hWnd) override;
  BOOL OnInitDialog(CWindow focus, LPARAM init_param);
  BOOL OnApply();

 private:
  Application* const application_;

  CButton monitor_cpu_;
  int cpu_usage_;
  CUpDownCtrl cpu_usage_spin_;
  int cpu_time_;
  CUpDownCtrl cpu_time_spin_;
  CComboBox cpu_time_unit_;
  CButton monitor_disk_;
  int disk_usage_;
  CUpDownCtrl disk_usage_spin_;
  int disk_time_;
  CUpDownCtrl disk_time_spin_;
  CComboBox disk_time_unit_;
  CButton monitor_traffic_;
  int traffic_;
  CUpDownCtrl traffic_spin_;
  CComboBox traffic_unit_;
  int traffic_time_;
  CUpDownCtrl traffic_time_spin_;
  CComboBox traffic_time_unit_;
};

#endif  // SHINOBU_MODULE_PERFMON_PERFORMANCE_MONITOR_PAGE_H_
