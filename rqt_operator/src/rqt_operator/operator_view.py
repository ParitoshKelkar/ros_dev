import os
import rospy
import numpy 

from qt_gui.plugin import Plugin
from python_qt_binding import loadUi
from python_qt_binding.QtGui import QWidget
from std_msgs.msg import String
from std_msgs.msg import Int32
from python_qt_binding.QtCore import Qt, QTimer, Signal, Slot
from rospy.numpy_msg import numpy_msg
from rospy_tutorials.msg import Floats


class operatorPlugin(Plugin):

    tapped = Signal(int)
    clearing = Signal()

    def __init__(self, context):
        super(operatorPlugin, self).__init__(context)
        # Give QObjects reasonable names
        self.setObjectName('operatorPluginObject')
        self.exp_flag = 1
        self.s = "Y"
        #print _exp_flag
        # Process standalone plugin command-line arguments
        from argparse import ArgumentParser
        parser = ArgumentParser()
        # Add argument(s) to the parser.
        parser.add_argument("-q", "--quiet", action="store_true",
                      dest="quiet",
                      help="Put plugin in silent mode")
        args, unknowns = parser.parse_known_args(context.argv())
        if not args.quiet:
            print 'arguments: ', args
            print 'unknowns: ', unknowns
        self.pub = rospy.Publisher("operator_to_bhavya",Int32,queue_size=10);
        self.sub = rospy.Subscriber("menu_num_pub", numpy_msg(Floats), self._callback)
        self.sub2 = rospy.Subscriber("PANDU_to_operator", Int32, self._callback2)

        # Create QWidget
        self._widget = QWidget()
        # Get path to UI file which is a sibling of this file
        # in this example the .ui and .py file are in the same folder
        ui_file = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'operatorView.ui')
        # Extend the widget with all attributes and children from UI file
        loadUi(ui_file, self._widget)
        # Give QObjects reasonable names
        self._widget.setObjectName('operatorPluginUi')
        # self._widget.pushButton.clicked.connect(self._test_click)
        # Show _widget.windowTitle on left-top of each plugin (when 
        # it's set in _widget). This is useful when you open multiple 
        # plugins at once. Also if you open multiple instances of your 
        # plugin at once, these lines add number to make it easy to 
        # tell from pane to pane.
        if context.serial_number() > 1:
            self._widget.setWindowTitle(self._widget.windowTitle() + (' (%d)' % context.serial_number()))
        # Add widget to the user interface
        context.add_widget(self._widget)
        self.tapped.connect(self.updateTableStat)
        self.clearing.connect(self._clearFields)
        self._widget.pushButton1.clicked.connect(self._deliver_t1)
        self._widget.pushButton2.clicked.connect(self._deliver_t2)
        
        self._widget.pushButton_home.clicked.connect(self._sendHome)
        inData = 0
        

    @Slot(int)
    def updateTableStat(self, st):
        TableNum = rospy.get_param('/TableNum')
        if st == 0:
            subs = "Samosa  :" + str(self.inData[0])
            subs2 = "\nPaneer Chilly  :" + str(self.inData[1])
            subs3 = "\nCheese  :" + str(self.inData[2])
            netS = subs + subs2 + subs3       
        elif st == 2:
            netS = 'Bill'
        elif st == 1:
            netS = 'Ketchup'
        elif st == 999:
            netS = 'Asked for assistance'

        if TableNum == 3:
            self._widget.textEdit1.setText(netS) 
        else:
            self._widget.textEdit2.setText(netS)

        pass

    @Slot()
    def _clearFields(self):
        TableNum = rospy.get_param('/TableNum')
        if TableNum == 3 :
            self._widget.textEdit1.setText('')
        else:
            self._widget.textEdit2.setText('')
        pass

    def _sendHome(self):
        self.pub.publish(999)
        pass

    def _deliver_t1(self):
        tempS = 'D1'
        self.pub.publish(3)
        self.clearing.emit()
        pass

    def _deliver_t2(self):
        tempS = 'D2'
        self.pub.publish(156)
        pass


    def shutdown_plugin(self):
        # TODO unregister all publishers here
        pass

    def _callback(self,data):
        self.inData = data.data
        self.tapped.emit(0)
        pass

    def _callback2(self, panduData):
       # self.st = panduData.data
       # print self.st
        self.tapped.emit(panduData.data)
#        print panduData.data
        pass

    def save_settings(self, plugin_settings, instance_settings):
        # TODO save intrinsic configuration, usually using:
        # instance_settings.set_value(k, v)
        pass

    def restore_settings(self, plugin_settings, instance_settings):
        # TODO restore intrinsic configuration, usually using:
        # v = instance_settings.value(k)
        pass

    #def trigger_configuration(self):
        # Comment in to signal that the plugin has a way to configure
        # This will enable a setting button (gear icon) in each dock widget title bar
        # Usually used to open a modal configuration dialog

