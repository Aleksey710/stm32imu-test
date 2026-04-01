import Chart from 'chart.js/auto';

let chart;
let timeIndex = 0;

const MAX_POINTS = 300;

// состояние осей
const axesState = {
    x: true,
    y: true,
    z: true
};

let initialized = false;

export function initChart() {
    
    if (initialized)
    {
		//console.log('Chart setup...2');
		return;
	} else
	{
		initialized = true;
		//console.log('Chart setup...1');
	}

    const ctx = document.getElementById('chart').getContext('2d');

    chart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [
                {
                    label: 'X',
                    data: [],
                    borderColor: 'red',
                    hidden: false
                },
                {
                    label: 'Y',
                    data: [],
                    borderColor: 'green',
                    hidden: false
                },
                {
                    label: 'Z',
                    data: [],
                    borderColor: 'blue',
                    hidden: false
                }
            ]
        },
        options: {
            animation: false,
            //parsing: false, // пропадает график . Разобраться
			normalized: true,
            responsive: true,
            //------------
            pointRadius: 1, 
			borderWidth: 1, 
			tension: 0,
            //------------
            interaction: {
                intersect: false,
                mode: 'nearest'
            },
            plugins: {
                legend: {
                    display: true
                }
            },
            scales: {
                x: {
                    display: true
                },
                y: {
                    display: true,
                    beginAtZero: false
                }
            }
        }
    });

    createControls();
}

export function dataUpdate(data) {
    if (!chart) return;

    // ось времени
    chart.data.labels.push(timeIndex++);

    // данные
    chart.data.datasets[0].data.push(data.x);
    chart.data.datasets[1].data.push(data.y);
    chart.data.datasets[2].data.push(data.z);

    // ограничение размера (как осциллограф)
    if (chart.data.labels.length > MAX_POINTS) {
        chart.data.labels.shift();

        chart.data.datasets.forEach(ds => {
            ds.data.shift();
        });
    }

    // авто-масштабирование
    autoScale();
    //updateMinMax(data);

    chart.update('none');
    //requestAnimationFrame(() => chart.update('none'));
}

// 📈 авто масштаб
function autoScale() {
    let allValues = [];

    chart.data.datasets.forEach((ds, i) => {
        if (!ds.hidden) {
            allValues = allValues.concat(ds.data);
        }
    });

    if (allValues.length === 0) return;

    const min = Math.min(...allValues);
    const max = Math.max(...allValues);

    const padding = (max - min) * 0.1 || 1;

    chart.options.scales.y.min = min - padding;
    chart.options.scales.y.max = max + padding;
}
/*
let minY = Infinity;
let maxY = -Infinity;

function updateMinMax(data) {
    const values = [data.x, data.y, data.z];

    for (const v of values) {
        if (v < minY) minY = v;
        if (v > maxY) maxY = v;
    }

    const padding = (maxY - minY) * 0.1 || 1;

    chart.options.scales.y.min = minY - padding;
    chart.options.scales.y.max = maxY + padding;
}
*/
// 🎛 UI для включения/выключения осей
function createControls() {
    const container = document.createElement('div');
    container.style.margin = '10px';

    ['x', 'y', 'z'].forEach((axis, index) => {
        const label = document.createElement('label');
        label.style.marginRight = '10px';

        const checkbox = document.createElement('input');
        checkbox.type = 'checkbox';
        checkbox.checked = true;

        checkbox.onchange = () => {
            axesState[axis] = checkbox.checked;
            chart.data.datasets[index].hidden = !checkbox.checked;
            chart.update();
        };

        label.appendChild(checkbox);
        label.appendChild(document.createTextNode(` ${axis.toUpperCase()}`));

        container.appendChild(label);
    });

	const chartEl = document.getElementById('chart');
    //document.body.
    chartEl.parentNode.insertBefore(container, document.getElementById('chart'));
}




