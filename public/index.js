    // Create dynamic connections between nodes
    const nodes = document.querySelectorAll('.node');
    const container = document.querySelector('.network-container');

    nodes.forEach((node, i) => {
      nodes.forEach((targetNode, j) => {
        if (i < j) {
          const connection = document.createElement('div');
          connection.className = 'connection';

          const rect1 = node.getBoundingClientRect();
          const rect2 = targetNode.getBoundingClientRect();
          const containerRect = container.getBoundingClientRect();

          const x1 = rect1.left - containerRect.left + rect1.width / 2;
          const y1 = rect1.top - containerRect.top + rect1.height / 2;
          const x2 = rect2.left - containerRect.left + rect2.width / 2;
          const y2 = rect2.top - containerRect.top + rect2.height / 2;

          const distance = Math.sqrt(Math.pow(x2 - x1, 2) + Math.pow(y2 - y1, 2));
          const angle = Math.atan2(y2 - y1, x2 - x1) * 180 / Math.PI;

          connection.style.width = distance + 'px';
          connection.style.left = x1 + 'px';
          connection.style.top = y1 + 'px';
          connection.style.transform = `rotate(${angle}deg)`;
          connection.style.animationDelay = (i * 0.3) + 's';

          container.appendChild(connection);
        }
      });
    });

    function scrollToInfo() {
      document.getElementById('info').scrollIntoView({behavior: 'smooth'});
    }
 
