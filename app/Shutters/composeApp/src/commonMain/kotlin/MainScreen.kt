import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material.BottomNavigation
import androidx.compose.material.BottomNavigationItem
import androidx.compose.material.Icon
import androidx.compose.material.SnackbarHost
import androidx.compose.material.SnackbarHostState
import androidx.compose.material.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import automations.AutomationsTab
import cafe.adriel.voyager.core.screen.Screen
import cafe.adriel.voyager.navigator.tab.CurrentTab
import cafe.adriel.voyager.navigator.tab.LocalTabNavigator
import cafe.adriel.voyager.navigator.tab.TabDisposable
import cafe.adriel.voyager.navigator.tab.TabNavigator
import controls.ControlsTab
import settings.SettingsTab

class MainScreen: Screen {
    @Composable
    override fun Content() {
        val snackbarHostState = remember { SnackbarHostState() }

        TabNavigator(
            ControlsTab,
            tabDisposable = {
                TabDisposable(
                    it,
                    listOf(ControlsTab, AutomationsTab, SettingsTab)
                )
            }
        ) {
            Box(modifier = Modifier.fillMaxSize()) {
                Box(modifier = Modifier.padding(bottom = 56.dp)) {
                    CurrentTab()
                }
                BottomNavigation(
                    modifier = Modifier
                        .align(Alignment.BottomStart)
                        .fillMaxWidth()
                ) {
                    val tabNavigator = LocalTabNavigator.current

                    BottomNavigationItem(
                        selected = tabNavigator.current.key == ControlsTab.key,
                        label = { Text(ControlsTab.options.title) },
                        icon = {
                            Icon(
                                painter = ControlsTab.options.icon!!,
                                contentDescription = ControlsTab.options.title
                            )
                        },
                        onClick = { tabNavigator.current = ControlsTab }
                    )

                    BottomNavigationItem(
                        selected = tabNavigator.current.key == AutomationsTab.key,
                        label = { Text(AutomationsTab.options.title) },
                        icon = {
                            Icon(
                                painter = AutomationsTab.options.icon!!,
                                contentDescription = AutomationsTab.options.title
                            )
                        },
                        onClick = { tabNavigator.current = AutomationsTab }
                    )

                    BottomNavigationItem(
                        selected = tabNavigator.current.key == SettingsTab.key,
                        label = { Text(SettingsTab.options.title) },
                        icon = {
                            Icon(
                                painter = SettingsTab.options.icon!!,
                                contentDescription = SettingsTab.options.title
                            )
                        },
                        onClick = { tabNavigator.current = SettingsTab }
                    )
                }
            }
        }
    }
}